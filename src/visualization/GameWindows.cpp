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
    // Inicializar límites del mundo (basado en chunks cargados)
    float worldSize = _chunkManager.GetLoadedChunkCount() * _chunkManager.getChunkSize() * 2.0f;
    sf::FloatRect worldBounds(-worldSize, -worldSize, worldSize * 2, worldSize * 2);
    
    _cameraController = std::make_unique<CameraController>(sf::Vector2f(0, 0), worldBounds);
    _mapRenderer = std::make_unique<MapRenderer>(_chunkManager);
    _inputHandler = std::make_unique<InputHandler>();
    
    // Configurar callbacks del input handler
    _inputHandler->setCameraMoveCallback([this](const sf::Vector2f& worldPos) {
        _cameraController->setPosition(worldPos);
    });
    
    _inputHandler->setZoomCallback([this](float factor) {
        _cameraController->zoom(factor);
        // Actualizar el tamaño de la vista después del zoom
        sf::Vector2u windowSize = _window.getSize();
        _cameraController->setViewSize(sf::Vector2f(windowSize.x, windowSize.y));
    });
    
    _inputHandler->setTileClickCallback([this](const sf::Vector2f& worldPos) {
        std::cout << "Tile clicked at world position: (" 
                  << worldPos.x << ", " << worldPos.y << ")\n";
        
        try {
            const Tile& tile = _chunkManager.GetTile(static_cast<int>(worldPos.x), 
                                                   static_cast<int>(worldPos.y));
            std::cout << "  Biome ID: " << tile.getBiomeId() 
                      << ", Has Water: " << tile.hasWater() << "\n";
        } catch (const std::exception& e) {
            std::cout << "  Error accessing tile: " << e.what() << "\n";
        }
    });
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
            // Actualizar la vista para que se adapte al nuevo tamaño de ventana
            sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
            _window.setView(sf::View(visibleArea));
        }
        
        _inputHandler->handleEvent(event, _window, *_cameraController);
    }
}

void GameWindow::update(float deltaTime) {
    _inputHandler->update(deltaTime);
}

void GameWindow::render() {
    _window.clear(sf::Color(30, 30, 30)); // Color de fondo oscuro
    
    // Actualizar el tamaño de la vista de la cámara basado en el tamaño actual de la ventana y el zoom
    sf::Vector2u windowSize = _window.getSize();
    _cameraController->setViewSize(sf::Vector2f(windowSize.x, windowSize.y));
    
    // Renderizar el mapa con la vista de la cámara
    _window.setView(_cameraController->getView());
    _mapRenderer->render(_window, *_cameraController);
    
    // Opcional: Dibujar UI aquí con vista por defecto
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