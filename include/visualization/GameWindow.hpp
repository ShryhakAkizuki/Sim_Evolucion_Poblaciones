#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include "visualization/MapRenderer.hpp"
#include "visualization/InputHandler.hpp"
#include "visualization/CameraController.hpp"
#include "map/ChunkManager.hpp"

class GameWindow {
private:
    sf::RenderWindow _window;
    std::unique_ptr<MapRenderer> _mapRenderer;
    std::unique_ptr<InputHandler> _inputHandler;
    std::unique_ptr<CameraController> _cameraController;
    
    ChunkManager& _chunkManager;
    bool _isRunning;

    void initializeWindow(uint32_t width, uint32_t height);
    void initializeComponents();
    void handleEvents();
    void update(float deltaTime);
    void render();

public:
    GameWindow(ChunkManager& chunkManager, uint32_t width = 800, uint32_t height = 600);
    ~GameWindow();

    void run();
    void close();
};