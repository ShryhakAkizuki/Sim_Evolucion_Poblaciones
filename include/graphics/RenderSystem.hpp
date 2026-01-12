#pragma once

#include <optional>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "graphics/Config.hpp"
#include "graphics/Camera.hpp"
#include "graphics/InputManager.hpp"
#include "graphics/TileRenderer.hpp"

class RenderSystem {
private:
    // ----- Atributos -----
    // Ventana y contexto
    GLFWwindow* _window = nullptr;
    
    // Configuración
    GraphicsConfig _config;
    int _chunksize = 16;

    // Subsistemas
    Camera _camera;
    InputManager _inputManager;
    TileRenderer _tileRenderer;
    
public:
    // ----- Constructores -----
    RenderSystem() = delete;  
    explicit RenderSystem(const GraphicsConfig& config, int chunksize = 16); 
    RenderSystem(const RenderSystem&) = delete;
    
    // ----- Destructor -----
    ~RenderSystem();
    
    // ----- Operadores -----
    RenderSystem& operator=(const RenderSystem&) = delete;
    
    // ----- Métodos -----
    // Inicializacion
    bool init();
    void shutdown();

    // Renderizado
    void renderFrame();
    
    // Procesamiento de entrada
    void processInput(float deltaTime);

    // Métodos de chunks
    void updateChunk(const ChunkCoord& coord, const glm::vec4* colors);
    void removeChunk(const ChunkCoord& coord);
    
    // Getters
    bool shouldClose() const { return glfwWindowShouldClose(_window); }
    Camera& getCamera() { return _camera; }
    InputManager& getInputManager() { return _inputManager; }
    TileRenderer& getTileRenderer() { return _tileRenderer; }

private:
    // Callbacks -> InputManager y Camara
    void setupCallbacks();
};