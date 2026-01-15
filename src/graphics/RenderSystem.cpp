#include <iostream>

#include "graphics/RenderSystem.hpp"

// ----- Constructores -----
RenderSystem::RenderSystem(const GraphicsConfig& config, const DynamicArray<glm::vec4>& BiomeColors, int chunksize) 
    : _config(config), _BiomeColors(BiomeColors), _chunksize(chunksize)  {}

// ----- Destructor -----
RenderSystem::~RenderSystem() {
    shutdown();
}

// ----- Métodos -----
// Inicializacion
bool RenderSystem::init() {
    
    // GLFW
    if (!glfwInit()) {
        std::cerr << "[RenderSystem] ERROR: No se pudo inicializar GLFW\n";
        return false;
    }    

    std::cout << "[RenderSystem] GLFW inicializado OK\n";

    // Ventana
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    std::cout << "[RenderSystem] Creando ventana " 
              << _config.windowWidth << "x" << _config.windowHeight << "...\n";

    _window = glfwCreateWindow(_config.windowWidth, _config.windowHeight, 
                              _config.windowTitle, nullptr, nullptr);
    
    if (!_window) {
        std::cerr << "[RenderSystem] ERROR: No se pudo crear ventana GLFW\n";
        glfwTerminate();
        return false;
    }    

    std::cout << "[RenderSystem] Ventana creada OK\n";

    glfwMakeContextCurrent(_window);
    std::cout << "[RenderSystem] Contexto OpenGL establecido\n";

    glfwSwapInterval(_config.vsync ? 1 : 0);
    
    std::cout << "[RenderSystem] Cargando GLAD...\n";
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "[RenderSystem] ERROR: No se pudo cargar GLAD\n";
        glfwDestroyWindow(_window);
        glfwTerminate();
        return false;
    }
    std::cout << "[RenderSystem] GLAD cargado OK\n";
    std::cout << "[RenderSystem] OpenGL Vendor: " << glGetString(GL_VENDOR) << "\n";
    std::cout << "[RenderSystem] OpenGL Renderer: " << glGetString(GL_RENDERER) << "\n";
    std::cout << "[RenderSystem] OpenGL Version: " << glGetString(GL_VERSION) << "\n";    

    // OpenGL básico
    glViewport(0, 0, _config.windowWidth, _config.windowHeight);
    glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Verificar errores OpenGL
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "[RenderSystem] ERROR OpenGL después de setup: " << err << "\n";
    }

    std::cout << "[RenderSystem] OpenGL configurado OK\n";

    // Subsistemas
    std::cout << "[RenderSystem] Configurando cámara...\n";
    _camera.setViewportSize(_config.windowWidth, _config.windowHeight);
    _camera.setZoomBound(_config.minZoom, _config.maxZoom);
    std::cout << "[RenderSystem] Configurando InputManager...\n";
    _inputManager = InputManager(_window);
    
    std::cout << "[RenderSystem] Configurando TileRenderer...\n";
    _tileRenderer = TileRenderer(_chunksize, _config.tileSize);
    if (!_tileRenderer.init()){
        std::cerr << "[RenderSystem] ERROR TileRenderer, no se pudo inicializar" << err << "\n";
    }

    // Callbacks
    setupCallbacks();
    
    std::cout << "[RenderSystem] Inicialización completada\n";
    return true;
}

void RenderSystem::shutdown() {
    if (_window) {
        glfwDestroyWindow(_window);
        glfwTerminate();
        _window = nullptr;
    }
}

// Renderizado
void RenderSystem::renderFrame() {
    // Clear
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Actualizar cámara
    _camera.update();
    
    // Renderizar tiles
    _tileRenderer.render(_camera, _config);
    
    // Swap
    glfwSwapBuffers(_window);
    
    // Poll events
    glfwPollEvents();
    _inputManager.update();
}

void RenderSystem::processInput(float deltaTime) {
    // Cerrar con ESC
    if (_inputManager.isKeyPressed(GLFW_KEY_ESCAPE)) {
        glfwSetWindowShouldClose(_window, true);
    }
    
    // Movimiento cámara
    float baseSpeed = _config.cameraMoveSpeed * deltaTime;
    float adjustedSpeed = _camera.getAdjustedSpeed(baseSpeed);

    if (_inputManager.isKeyDown(GLFW_KEY_W)) _camera.move(0, adjustedSpeed);
    if (_inputManager.isKeyDown(GLFW_KEY_S)) _camera.move(0, -adjustedSpeed);
    if (_inputManager.isKeyDown(GLFW_KEY_A)) _camera.move(-adjustedSpeed, 0);
    if (_inputManager.isKeyDown(GLFW_KEY_D)) _camera.move(adjustedSpeed, 0);
    
    // Zoom
    float scroll = _inputManager.getMouseScroll();
    if (scroll != 0.0f) {
        _camera.zoom(1.0f + (_config.cameraZoomSpeed * scroll));
    }
}

// Métodos de chunks
void RenderSystem::updateChunk(const ChunkCoord& coord, const DynamicArray<DynamicArray<Tile>>& Chunk) {
    _tileRenderer.updateChunk(coord, TiletoColor(Chunk).data());
}

void RenderSystem::removeChunk(const ChunkCoord& coord) {
    _tileRenderer.removeChunk(coord);
}

// Carga y descarga masiva
void RenderSystem::updateChunk(const DynamicArray<ChunkCoord> &Coord_Array, DynamicArray<const DynamicArray<DynamicArray<Tile>>*> Chunk_list) {
    if(Coord_Array.size() != Chunk_list.size()) return;

    for(int i = 0; i < static_cast<int>(Chunk_list.size()); ++i){
        if(Chunk_list[i]!= nullptr){
            updateChunk(Coord_Array[i],*(Chunk_list[i]));
        }
    }
}

void RenderSystem::removeChunk(const DynamicArray<ChunkCoord> &Coord_Array) {
    for(int i = 0; i < static_cast<int>(Coord_Array.size()); ++i){
        _tileRenderer.removeChunk(Coord_Array[i]);
    }
}

// Conversion
DynamicArray<glm::vec4> RenderSystem::TiletoColor(const DynamicArray<DynamicArray<Tile>>& Chunk) {
    DynamicArray<glm::vec4> TileColors(Chunk.size()*Chunk.size());

    for (uint32_t y = 0; y < Chunk.size(); ++y) {
        for (uint32_t x = 0; x < Chunk[y].size(); ++x) {
            const Tile& tile = Chunk[y][x];
            glm::vec4 color;

            if(tile.hasWater()) color = glm::vec4(0.0f, 0.3f, 0.8f, 0.7f);
            else color = _BiomeColors[tile.getBiomeId()];

            TileColors[y * Chunk[y].size() + x] = color;
        }
    }
    return TileColors;
}

// Callbacks -> InputManager y Camara
void RenderSystem::setupCallbacks() {
    glfwSetWindowUserPointer(_window, this);
    
    // Tamaño de ventana
    glfwSetFramebufferSizeCallback(_window, [](GLFWwindow* window, int w, int h) {
        glViewport(0, 0, w, h);
        RenderSystem* rs = static_cast<RenderSystem*>(glfwGetWindowUserPointer(window));
        rs->getCamera().setViewportSize(w, h);
    });
    
    // Teclado
    glfwSetKeyCallback(_window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        RenderSystem* rs = static_cast<RenderSystem*>(glfwGetWindowUserPointer(window));
        rs->getInputManager().processKeyEvent(key, action);
    });
    
    // Ratón
    glfwSetMouseButtonCallback(_window, [](GLFWwindow* window, int button, int action, int mods) {
        RenderSystem* rs = static_cast<RenderSystem*>(glfwGetWindowUserPointer(window));
        rs->getInputManager().processMouseButtonEvent(button, action);
    });
    
    glfwSetCursorPosCallback(_window, [](GLFWwindow* window, double x, double y) {
        RenderSystem* rs = static_cast<RenderSystem*>(glfwGetWindowUserPointer(window));
        rs->getInputManager().processMouseMoveEvent(x, y);
    });
    
    glfwSetScrollCallback(_window, [](GLFWwindow* window, double x, double y) {
        RenderSystem* rs = static_cast<RenderSystem*>(glfwGetWindowUserPointer(window));
        rs->getInputManager().processMouseScrollEvent(x, y);
    });
}