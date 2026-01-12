#include <iostream>
#include <cmath>
#include <algorithm>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "graphics/TileRenderer.hpp"

// ----- Shader Sources -----
const char* vertexShaderSource = R"glsl(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec4 aColor;

out vec4 fragColor;

uniform mat4 uViewProjection;

void main() {
    gl_Position = uViewProjection * vec4(aPos, 0.0, 1.0);
    fragColor = aColor;
}
)glsl";

const char* fragmentShaderSource = R"glsl(
#version 330 core
in vec4 fragColor;
out vec4 FragColor;

void main() {
    FragColor = fragColor;
}
)glsl";

const char* wireframeVertexShaderSource = R"glsl(
#version 330 core
layout (location = 0) in vec2 aPos;

uniform mat4 uViewProjection;

void main() {
    gl_Position = uViewProjection * vec4(aPos, 0.0, 1.0);
}
)glsl";

const char* wireframeFragmentShaderSource = R"glsl(
#version 330 core
out vec4 FragColor;
uniform vec4 uColor;

void main() {
    FragColor = uColor;
}
)glsl";

// ----- Constructores -----
TileRenderer::TileRenderer(int chunkSize, int tileSize)
    : _tileSize(tileSize),
      _chunkSize(chunkSize) {
    
    _vertexBuffer.reserve(_chunkSize * _chunkSize * 4 * 6);
    _indexBuffer.reserve(_chunkSize * _chunkSize * 6);
    
    _initialized = false;
}

TileRenderer::TileRenderer(TileRenderer&& other) noexcept 
    : _shaderProgram(other._shaderProgram)
    , _wireframeShaderProgram(other._wireframeShaderProgram)
    , _chunkCache(std::move(other._chunkCache))
    , _initialized(other._initialized)
    , _tileSize(other._tileSize)
    , _chunkSize(other._chunkSize)
    , _wireframeMode(other._wireframeMode)
    , _stats(std::move(other._stats))
    , _vertexBuffer(std::move(other._vertexBuffer))
    , _indexBuffer(std::move(other._indexBuffer)) {
    
    // Invalidar recursos en el objeto movido
    other._shaderProgram = 0;
    other._wireframeShaderProgram = 0;
    other._initialized = false;
}

// ----- Destructor -----

TileRenderer::~TileRenderer() {
    std::cout << "[TileRenderer] Destructor\n";
    
    if (_initialized) {
        shutdown();
    }
    
    std::cout << "[TileRenderer] Objeto destruido\n";
}

// ----- Operadores -----

TileRenderer& TileRenderer::operator=(TileRenderer&& other) noexcept {
    if (this != &other) {
        // Limpiar recursos actuales
        shutdown();
        
        // Mover recursos
        _shaderProgram = other._shaderProgram;
        _wireframeShaderProgram = other._wireframeShaderProgram;
        _chunkCache = std::move(other._chunkCache);
        _tileSize = other._tileSize;
        _chunkSize = other._chunkSize;
        _wireframeMode = other._wireframeMode;
        _initialized = other._initialized;
        _stats = std::move(other._stats);
        _vertexBuffer = std::move(other._vertexBuffer);
        _indexBuffer = std::move(other._indexBuffer);
        
        // Invalidar recursos en el objeto movido
        other._shaderProgram = 0;
        other._wireframeShaderProgram = 0;
        other._initialized = false;
    }
    return *this;
}

// ----- Métodos Publicos -----
// Inicializacion tardia - OpenGL context
bool TileRenderer::init() {
    if (_initialized) {
        std::cout << "[TileRenderer] Ya está inicializado\n";
        return true;
    }
    
    std::cout << "[TileRenderer] Inicializando OpenGL...\n";
    
    // 1. Verificar contexto OpenGL
    GLFWwindow* currentContext = glfwGetCurrentContext();
    if (!currentContext) {
        std::cerr << "[TileRenderer] ERROR: No hay contexto OpenGL activo!\n";
        return false;
    }

    // 2. Verificar GLAD (si no está cargado)
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "[TileRenderer] ERROR: No se pudo cargar GLAD!\n";
        return false;
    }

    std::cout << "[TileRenderer] Contexto OpenGL: OK\n";
    std::cout << "[TileRenderer] GLAD: OK\n";
    std::cout << "[TileRenderer] OpenGL Version: " << glGetString(GL_VERSION) << "\n";

    // 3. Inicializar shaders
    try {
        initializeShaders();
        std::cout << "[TileRenderer] Shaders: OK\n";
    } catch (const std::exception& e) {
        std::cerr << "[TileRenderer] ERROR al inicializar shaders: " << e.what() << "\n";
        return false;
    }
    
    _initialized = true;
    std::cout << "[TileRenderer] Inicialización completada exitosamente\n";
    return true;
}

void TileRenderer::shutdown() {
    std::cout << "[TileRenderer] Cerrando recursos...\n";
    
    if (!_initialized) {
        std::cout << "[TileRenderer] No estaba inicializado, nada que cerrar\n";
        return;
    }
    
    // 1. Limpiar caché de chunks
    clearCache();
    
    // 2. Eliminar shaders
    if (_shaderProgram) {
        glDeleteProgram(_shaderProgram);
        _shaderProgram = 0;
    }
    
    if (_wireframeShaderProgram) {
        glDeleteProgram(_wireframeShaderProgram);
        _wireframeShaderProgram = 0;
    }
    
    // 3. Limpiar buffers temporales
    _vertexBuffer.clear();
    _indexBuffer.clear();
    
    _initialized = false;
    std::cout << "[TileRenderer] Recursos OpenGL liberados\n";
}

// Renderizado principal    
void TileRenderer::render(const Camera& camera, const GraphicsConfig& config) {
    // Resetear estadísticas para este frame
    _stats = Stats();
    
    // Renderizar chunks visibles
    renderVisibleChunks(camera);
    
    if (config.showChunkBorders) renderChunkBorders(camera, config);    
    if (config.showGrid) renderGrid(camera, config);
    
    _stats.drawCalls++;
}

// Gestión de Caché Externa
void TileRenderer::updateChunk(const ChunkCoord& coord, const glm::vec4* tileColors) {
    ChunkRenderData& data = _chunkCache[coord];
    updateChunkData(coord, data, tileColors);
}

void TileRenderer::removeChunk(const ChunkCoord& coord) {
    ChunkRenderData* data = _chunkCache.find_ptr(coord);
    if (data) {
        cleanupChunkData(*data);
        _chunkCache.erase(coord);
    }
}

void TileRenderer::invalidateChunk(const ChunkCoord& coord) {
    ChunkRenderData* data = _chunkCache.find_ptr(coord);
    if (data) {
        data->dirty = true;
    }
}

void TileRenderer::clearCache() {
    for (auto& pair : _chunkCache) {
        cleanupChunkData(pair.second());
    }
    _chunkCache.clear();
}

// Configuración 
void TileRenderer::setTileSize(int size) {
    if (size != _tileSize) {
        _tileSize = size;
        
        if (_initialized) {
            for (auto& pair : _chunkCache) {
                pair.second().dirty = true;
            }
        }
    }
}

void TileRenderer::setChunkSize(int size) {
    if (size != _chunkSize) {
        _chunkSize = size;
        
        if (_initialized) {
            clearCache();
            _vertexBuffer.reserve(_chunkSize * _chunkSize * 4 * 6);
            _indexBuffer.reserve(_chunkSize * _chunkSize * 6);
        } else {
            // Si no está inicializado, solo reservar memoria
            _vertexBuffer.reserve(_chunkSize * _chunkSize * 4 * 6);
            _indexBuffer.reserve(_chunkSize * _chunkSize * 6);
        }
    }
}

// ----- Métodos Privados -----
// Renderizado interno 
void TileRenderer::renderVisibleChunks(const Camera& camera) {
    // Calcular bounds visibles
    glm::vec2 minBounds, maxBounds;
    calculateVisibleBounds(camera, minBounds, maxBounds);
    
    // Calcular chunks visibles
    float worldChunkSize = static_cast<float>(_tileSize) * static_cast<float>(_chunkSize);
    
    int startChunkX = static_cast<int>(std::floor(minBounds.x / worldChunkSize));
    int startChunkY = static_cast<int>(std::floor(minBounds.y / worldChunkSize));
    int endChunkX = static_cast<int>(std::ceil(maxBounds.x / worldChunkSize));
    int endChunkY = static_cast<int>(std::ceil(maxBounds.y / worldChunkSize));
    
    // Usar shader normal o wireframe
    GLuint currentShader = _wireframeMode ? _wireframeShaderProgram : _shaderProgram;
    glUseProgram(currentShader);
    
    // Pasar matriz de vista-proyección
    GLint uViewProjection = glGetUniformLocation(currentShader, "uViewProjection");
    if (uViewProjection != -1) {
        glUniformMatrix4fv(uViewProjection, 1, GL_FALSE, 
                          &camera.getViewProjectionMatrix()[0][0]);
    }
    
    for (auto& pair : _chunkCache) {
        const ChunkCoord& coord = pair.first();
        ChunkRenderData& renderData = pair.second();
        
        // Verificar si el chunk está dentro del área visible
        int chunkX = coord.x();
        int chunkY = coord.y();
        
        if (chunkX >= startChunkX && chunkX <= endChunkX &&
            chunkY >= startChunkY && chunkY <= endChunkY) {
            
            // Renderizar si tiene geometría
            if (renderData.vao && renderData.vertexCount > 0) {
                glBindVertexArray(renderData.vao);
                
                if (_wireframeMode) {
                    GLint uColor = glGetUniformLocation(currentShader, "uColor");
                    if (uColor != -1) {
                        glm::vec4 wireframeColor(0.8f, 0.8f, 0.8f, 1.0f);
                        glUniform4fv(uColor, 1, &wireframeColor[0]);
                    }
                    
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                    glDrawElements(GL_TRIANGLES, renderData.vertexCount, 
                                  GL_UNSIGNED_INT, 0);
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                } else {
                    glDrawElements(GL_TRIANGLES, renderData.vertexCount, 
                                  GL_UNSIGNED_INT, 0);
                }
                
                glBindVertexArray(0);
                
                _stats.renderedChunks++;
                _stats.renderedTiles += _chunkSize * _chunkSize;
            }
        }
    }
    
    glUseProgram(0);
}

void TileRenderer::renderChunkBorders(const Camera& camera, const GraphicsConfig& config) {
    // Implementación similar pero usando la cámara pasada como parámetro
    glUseProgram(_wireframeShaderProgram);
    
    GLint uColor = glGetUniformLocation(_wireframeShaderProgram, "uColor");
    if (uColor != -1) {
        glm::vec4 borderColor(
            config.colors.chunkBorderColor[0],
            config.colors.chunkBorderColor[1],
            config.colors.chunkBorderColor[2],
            config.colors.chunkBorderColor[3]
        );
        glUniform4fv(uColor, 1, &borderColor[0]);
    }
    
    GLint uViewProjection = glGetUniformLocation(_wireframeShaderProgram, "uViewProjection");
    if (uViewProjection != -1) {
        glUniformMatrix4fv(uViewProjection, 1, GL_FALSE, 
                          &camera.getViewProjectionMatrix()[0][0]);
    }
    
    // Dibujar bordes...
    glUseProgram(0);
}

void TileRenderer::renderGrid(const Camera& camera, const GraphicsConfig& config) {
    // Temporal: implementación básica
    glUseProgram(_wireframeShaderProgram);
    
    // Configurar color desde config
    GLint uColor = glGetUniformLocation(_wireframeShaderProgram, "uColor");
    if (uColor != -1) {
        glm::vec4 gridColor(
            config.colors.gridColor[0],
            config.colors.gridColor[1],
            config.colors.gridColor[2],
            config.colors.gridColor[3]
        );
        glUniform4fv(uColor, 1, &gridColor[0]);
    }
    
    // Pasar matriz de vista-proyección
    GLint uViewProjection = glGetUniformLocation(_wireframeShaderProgram, "uViewProjection");
    if (uViewProjection != -1) {
        glUniformMatrix4fv(uViewProjection, 1, GL_FALSE, 
                          &camera.getViewProjectionMatrix()[0][0]);
    }
    
    // Dibujar líneas de grid
    // (Implementación detallada requeriría crear geometría de grid)
    
    glUseProgram(0);
}

// Gestión de caché
void TileRenderer::updateChunkData(const ChunkCoord& coord, 
                                  ChunkRenderData& data, 
                                  const glm::vec4* tileColors) {
    // Limpiar recursos antiguos
    cleanupChunkData(data);
    
    // Crear nueva geometría para el chunk
    _vertexBuffer.clear();
    _indexBuffer.clear();
    
    // Calcular posición base del chunk
    float baseX = static_cast<float>(coord.x()) * 
                  static_cast<float>(_chunkSize) * _tileSize;
    float baseY = static_cast<float>(coord.y()) * 
                  static_cast<float>(_chunkSize) * _tileSize;
    
    // Generar geometría para cada tile
    uint32_t vertexOffset = 0;
    
    for (int tileY = 0; tileY < _chunkSize; ++tileY) {
        for (int tileX = 0; tileX < _chunkSize; ++tileX) {
            // Coordenadas del tile
            float x1 = baseX + tileX * _tileSize;
            float y1 = baseY + tileY * _tileSize;
            float x2 = x1 + _tileSize;
            float y2 = y1 + _tileSize;
            
            // Color del tile (desde array externo)
            glm::vec4 tileColor = tileColors[tileY * _chunkSize + tileX];
            
            // Agregar vértices
            _vertexBuffer.push_back(x1);
            _vertexBuffer.push_back(y1);
            _vertexBuffer.push_back(tileColor.r);
            _vertexBuffer.push_back(tileColor.g);
            _vertexBuffer.push_back(tileColor.b);
            _vertexBuffer.push_back(tileColor.a);
            
            _vertexBuffer.push_back(x2);
            _vertexBuffer.push_back(y1);
            _vertexBuffer.push_back(tileColor.r);
            _vertexBuffer.push_back(tileColor.g);
            _vertexBuffer.push_back(tileColor.b);
            _vertexBuffer.push_back(tileColor.a);
            
            _vertexBuffer.push_back(x2);
            _vertexBuffer.push_back(y2);
            _vertexBuffer.push_back(tileColor.r);
            _vertexBuffer.push_back(tileColor.g);
            _vertexBuffer.push_back(tileColor.b);
            _vertexBuffer.push_back(tileColor.a);
            
            _vertexBuffer.push_back(x1);
            _vertexBuffer.push_back(y2);
            _vertexBuffer.push_back(tileColor.r);
            _vertexBuffer.push_back(tileColor.g);
            _vertexBuffer.push_back(tileColor.b);
            _vertexBuffer.push_back(tileColor.a);
            
            // Agregar índices
            _indexBuffer.push_back(vertexOffset + 0);
            _indexBuffer.push_back(vertexOffset + 1);
            _indexBuffer.push_back(vertexOffset + 2);
            _indexBuffer.push_back(vertexOffset + 0);
            _indexBuffer.push_back(vertexOffset + 2);
            _indexBuffer.push_back(vertexOffset + 3);
            
            vertexOffset += 4;
        }
    }
    
    // Crear y configurar VAO/VBO/EBO
    glGenVertexArrays(1, &data.vao);
    glGenBuffers(1, &data.vbo);
    glGenBuffers(1, &data.ebo);
    
    glBindVertexArray(data.vao);
    
    // Configurar VBO
    glBindBuffer(GL_ARRAY_BUFFER, data.vbo);
    glBufferData(GL_ARRAY_BUFFER, 
                _vertexBuffer.size() * sizeof(float),
                _vertexBuffer.data(), 
                GL_STATIC_DRAW);
    
    // Configurar atributos
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 
                         (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // Configurar EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
                _indexBuffer.size() * sizeof(uint32_t),
                _indexBuffer.data(), 
                GL_STATIC_DRAW);
    
    glBindVertexArray(0);
    
    // Guardar información
    data.vertexCount = static_cast<uint32_t>(_indexBuffer.size());
    data.dirty = false;
    data.lastUpdate = 0;
}

void TileRenderer::cleanupChunkData(ChunkRenderData& data) {
    if (data.vao) glDeleteVertexArrays(1, &data.vao);
    if (data.vbo) glDeleteBuffers(1, &data.vbo);
    if (data.ebo) glDeleteBuffers(1, &data.ebo);
    
    data.vao = data.vbo = data.ebo = 0;
    data.vertexCount = 0;
}

// Cálculos
void TileRenderer::calculateVisibleBounds(const Camera& camera, 
                                         glm::vec2& min, glm::vec2& max) const {
    glm::vec2 viewportSize = camera.getViewportSize();
    glm::vec2 worldBottomLeft = camera.screenToWorld(0, 0);
    glm::vec2 worldTopRight = camera.screenToWorld(viewportSize.x, viewportSize.y);
    
    // Margen de 1 chunk extra
    float margin = _tileSize * _chunkSize;
    
    min.x = worldBottomLeft.x - margin;
    min.y = worldBottomLeft.y - margin;
    max.x = worldTopRight.x + margin;
    max.y = worldTopRight.y + margin;
}

// OpenGL 
GLuint TileRenderer::compileShader(const char* source, GLenum type) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    
    // Verificar compilación
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "[TileRenderer] ERROR en compilación de shader: " 
                  << infoLog << "\n";
        glDeleteShader(shader);
        return 0;
    }
    
    return shader;
}

GLuint TileRenderer::createShaderProgram(const char* vertexSource, 
                                       const char* fragmentSource) {
    GLuint vertexShader = compileShader(vertexSource, GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader(fragmentSource, GL_FRAGMENT_SHADER);
    
    if (!vertexShader || !fragmentShader) {
        return 0;
    }
    
    // Crear programa
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    
    // Verificar link
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "[TileRenderer] ERROR en link de shader program: " 
                  << infoLog << "\n";
        glDeleteProgram(program);
        program = 0;
    }
    
    // Limpiar shaders individuales
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return program;
}

void TileRenderer::initializeShaders() {
    std::cout << "[TileRenderer] Compilando shaders...\n";
    
    _shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);
    _wireframeShaderProgram = createShaderProgram(
        wireframeVertexShaderSource, 
        wireframeFragmentShaderSource
    );
    
    if (!_shaderProgram || !_wireframeShaderProgram) {
        throw std::runtime_error("Failed to compile shaders");
    }
    
    std::cout << "[TileRenderer] Shaders compilados exitosamente\n";
}