#include "visualization/GameWindow.hpp"
#include <iostream>

GameWindow::GameWindow(ChunkManager& chunkManager, uint32_t width, uint32_t height)
    : _chunkManager(chunkManager), _isRunning(false) {
    
    initializeWindow(width, height);
    initializeComponents();
    std::cout << "GameWindow inicializado correctamente\n";
}

GameWindow::~GameWindow() {
    close();
}

void GameWindow::initializeWindow(uint32_t width, uint32_t height) {
    _window.create(sf::VideoMode(width, height), "Procedural World Visualization - SFML");
    _window.setFramerateLimit(60);
    _window.setVerticalSyncEnabled(true);
}

void GameWindow::initializeComponents() {
    // Inicializar límites del mundo
    float worldSize = _chunkManager.GetLoadedChunkCount() * _chunkManager.getChunkSize() * 2.0f;
    sf::FloatRect worldBounds(-worldSize, -worldSize, worldSize * 2, worldSize * 2);
    
    _cameraController = std::make_unique<CameraController>(sf::Vector2f(0, 0), worldBounds);
    _mapRenderer = std::make_unique<MapRenderer>(_chunkManager);
    _inputHandler = std::make_unique<InputHandler>();
    
    // Asegurar que TileDrawer tenga la referencia actualizada al BiomeSystem
    auto worldGenerator = _chunkManager.getWorldGenerator();
    if (worldGenerator) {
        auto biomeSystem = worldGenerator->getBiomeSystem();
        _mapRenderer->setBiomeSystem(biomeSystem);
    }
    
    // Configurar callbacks
    _inputHandler->setCameraMoveCallback([this](const sf::Vector2f& worldPos) {
        _cameraController->setPosition(worldPos);
    });
    
    _inputHandler->setZoomCallback([this](float factor) {
        _cameraController->zoom(factor);
        sf::Vector2u windowSize = _window.getSize();
        _cameraController->setViewSize(sf::Vector2f(windowSize.x, windowSize.y));
    });
    
    _inputHandler->setTileClickCallback([this](const sf::Vector2f& worldPos) {
        std::cout << "Tile clicked at world position: (" 
                  << worldPos.x << ", " << worldPos.y << ")\n";
        
        try {
            const Tile& tile = _chunkManager.GetTile(static_cast<int>(worldPos.x), 
                                                   static_cast<int>(worldPos.y));
            
            // Obtener información del bioma
            auto worldGenerator = _chunkManager.getWorldGenerator();
            if (worldGenerator) {
                auto biomeSystem = worldGenerator->getBiomeSystem();
                auto biome = biomeSystem->getBioma(tile.getBiomeId());
                
                std::cout << "  Biome ID: " << tile.getBiomeId();
                if (biome) {
                    std::cout << " (" << biome->getNombre() << ")";
                    Color biomeColor = biome->getColor();
                    std::cout << " - Color: RGB(" << (int)biomeColor.r << ", " 
                              << (int)biomeColor.g << ", " << (int)biomeColor.b << ")";
                }
                std::cout << "\n";
            }
            
            std::cout << "  Has Water: " << tile.hasWater() << "\n";
            
        } catch (const std::exception& e) {
            std::cout << "  Error accessing tile: " << e.what() << "\n";
        }
    });
    
    // Configurar tamaño inicial de la vista
    sf::Vector2u windowSize = _window.getSize();
    _cameraController->setViewSize(sf::Vector2f(windowSize.x, windowSize.y));
}

void GameWindow::run() {
    if (!_window.isOpen()) return;
    
    _isRunning = true;
    sf::Clock clock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;
    const sf::Time timePerFrame = sf::seconds(1.f / 60.f);
    
    std::cout << "Iniciando bucle principal de GameWindow...\n";
    
    while (_isRunning && _window.isOpen()) {
        timeSinceLastUpdate += clock.restart();
        
        while (timeSinceLastUpdate > timePerFrame) {
            timeSinceLastUpdate -= timePerFrame;
            
            handleEvents();
            update(timePerFrame.asSeconds());
        }
        
        render();
    }
}

void GameWindow::handleEvents() {
    sf::Event event;
    while (_window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            close();
        }
        else if (event.type == sf::Event::Resized) {
            // Solo actualizar cuando la ventana cambie de tamaño
            sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
            _window.setView(sf::View(visibleArea));
            _cameraController->setViewSize(sf::Vector2f(event.size.width, event.size.height));
        }
        
        _inputHandler->handleEvent(event, _window, *_cameraController);
    }
}

void GameWindow::update(float deltaTime) {
    _inputHandler->update(deltaTime);
}

void GameWindow::render() {
    _window.clear(sf::Color(30, 30, 30));
    
    // NO actualizar el tamaño de la vista aquí - solo se hace en resize y zoom
    _window.setView(_cameraController->getView());
    _mapRenderer->render(_window, *_cameraController);
    
    _window.setView(_window.getDefaultView());
    _window.display();
}

void GameWindow::close() {
    _isRunning = false;
    if (_window.isOpen()) {
        _window.close();
    }
    std::cout << "GameWindow cerrado\n";
}