#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "graphics/Config.hpp"
#include "graphics/Camera.hpp"

#include "map/manager/ChunkCord.hpp"

#include "data_structures/Unordered_map.hpp"
#include "data_structures/DynamicArray.hpp"

struct Stats {
    uint32_t renderedChunks = 0;
    uint32_t renderedTiles = 0;
    uint32_t drawCalls = 0;
};

struct ChunkRenderData {
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;
    uint32_t vertexCount = 0;
    bool dirty = true;
    uint64_t lastUpdate = 0;
};

class TileRenderer {
private:
    // ----- Atributos -----
    // Recursos OpenGL
    GLuint _shaderProgram = 0;
    GLuint _wireframeShaderProgram = 0;
    
    // Caché de renderizado
    Unordered_map<ChunkCoord, ChunkRenderData> _chunkCache;
    
    // Estado
    bool _initialized = false;

    // Configuración 
    int _tileSize = 32;
    int _chunkSize = 16;
    bool _wireframeMode = false;
    
    // Estadísticas
    Stats _stats;
    
    // Buffers temporales
    DynamicArray<float> _vertexBuffer;
    DynamicArray<uint32_t> _indexBuffer;

public:
    // ----- Constructores -----
    TileRenderer(int chunkSize = 16, int tileSize = 32);
    
    TileRenderer(const TileRenderer&) = delete;
    TileRenderer(TileRenderer&& other) noexcept;

    // ----- Destructor -----
    ~TileRenderer();

    // ----- Operadores -----
    TileRenderer& operator=(const TileRenderer&) = delete;
    TileRenderer& operator=(TileRenderer&& other) noexcept;

    // ----- Métodos -----
    // Inicializacion tardia - OpenGL context
    bool init();                     
    void shutdown();                    
    bool isInitialized() const { return _initialized; }

    // Renderizado principal
    void render(const Camera& camera, const GraphicsConfig& config);
    
    // Gestión de Caché Externa 
    void updateChunk(const ChunkCoord& coord, const glm::vec4* tileColors);
    void removeChunk(const ChunkCoord& coord);
    void invalidateChunk(const ChunkCoord& coord);
    void clearCache();
 
    // Configuración 
    void setWireframeMode(bool enabled) { _wireframeMode = enabled; }
    void setTileSize(int size);
    void setChunkSize(int size);

    // Estadísticas 
    const Stats& getStats() const { return _stats; }
    size_t getCacheSize() const { return _chunkCache.size(); }
    void resetStats() { _stats = Stats(); }

private:
    // Renderizado interno 
    void renderVisibleChunks(const Camera& camera);
    void renderChunkBorders(const Camera& camera, const GraphicsConfig& config);
    void renderGrid(const Camera& camera, const GraphicsConfig& config);
    
    // Gestión de caché
    void updateChunkData(const ChunkCoord& coord, 
                        ChunkRenderData& data, 
                        const glm::vec4* tileColors);
    
    void cleanupChunkData(ChunkRenderData& data);

    // Cálculos
    void calculateVisibleBounds(const Camera& camera, 
                               glm::vec2& min, glm::vec2& max) const;
    
    // OpenGL 
    GLuint compileShader(const char* source, GLenum type);
    GLuint createShaderProgram(const char* vertexSource, 
                              const char* fragmentSource);
    void initializeShaders();
};