#define NOMINMAX

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <limits.h>
#endif

#include "biome/BiomeSystem.hpp"
#include "map/WorldSystem.hpp"
#include "graphics/RenderSystem.hpp"

#include <iostream>
#include <chrono>
#include <random>
#include <thread>

int main(){
    // Semilla Aleatoria
    std::random_device rd;
    auto time_seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    auto thread_id = std::hash<std::thread::id>{}(std::this_thread::get_id());
    
    uint64_t worldSeed = rd() ^ time_seed ^ (thread_id << 32);

    std::cout << "=== INICIALIZANDO SISTEMA ===\n";
    std::cout << "Semilla del mundo: " << worldSeed << "\n";

    // -------------------------- Motor de biomas --------------------------
    std::cout << "Cargando biomas...\n";
    BiomeSystem Biome_Engine(worldSeed);
    
    Biome_Engine.registrarBioma(std::make_unique<Bioma>(
        0, "Llanura", Color{124, 252, 0},
        22.0f, 45.0f, 70.0f, 1.0f, 0.2f, 0.2f,
        6.0f, 15.0f, 12.0f, 0.15f, 0.08f, 0.07f,
        static_cast<uint32_t>(worldSeed)
    ));
    
    Biome_Engine.registrarBioma(std::make_unique<Bioma>(
        1, "Bosque", Color{34, 139, 34},
        18.0f, 65.0f, 50.0f, 1.3f, 0.15f, 0.1f,
        4.0f, 12.0f, 10.0f, 0.25f, 0.06f, 0.04f,
        static_cast<uint32_t>(worldSeed)
    ));
    
    Biome_Engine.registrarBioma(std::make_unique<Bioma>(
        2, "Colinas", Color{139, 115, 85},
        16.0f, 55.0f, 65.0f, 1.4f, 0.1f, 0.15f,
        5.0f, 10.0f, 8.0f, 0.3f, 0.05f, 0.06f,
        static_cast<uint32_t>(worldSeed)
    ));
    
    Biome_Engine.registrarBioma(std::make_unique<Bioma>(
        3, "Pantano", Color{48, 128, 20},
        20.0f, 85.0f, 45.0f, 1.6f, 0.25f, 0.05f,
        3.0f, 8.0f, 6.0f, 0.4f, 0.12f, 0.02f,
        static_cast<uint32_t>(worldSeed)
    ));

    
    Biome_Engine.registrarBioma(std::make_unique<Bioma>(
        4, "Desierto", Color{255, 230, 150},
        35.0f, 15.0f, 85.0f, 1.5f, 0.05f, 0.3f,
        8.0f, 5.0f, 10.0f, 0.25f, 0.03f, 0.12f,
        static_cast<uint32_t>(worldSeed)
    ));
    
    Biome_Engine.registrarBioma(std::make_unique<Bioma>(
        5, "Sabana", Color{210, 180, 140},
        28.0f, 35.0f, 75.0f, 1.2f, 0.12f, 0.18f,
        7.0f, 10.0f, 9.0f, 0.2f, 0.07f, 0.08f,
        static_cast<uint32_t>(worldSeed)
    ));
    
    Biome_Engine.registrarBioma(std::make_unique<Bioma>(
        6, "Tundra", Color{200, 220, 240},
        5.0f, 60.0f, 40.0f, 1.3f, 0.08f, 0.12f,
        4.0f, 12.0f, 7.0f, 0.25f, 0.04f, 0.05f,
        static_cast<uint32_t>(worldSeed)
    ));

    // Tabla de conversión de biome ID a color
    const int numBiomes = Biome_Engine.getCantidadBiomas();
    DynamicArray<glm::vec4> biomeColors(numBiomes);
    
    for (int i = 0; i < numBiomes; ++i) {
        const Bioma* biome = Biome_Engine.getBioma(i);
        if (biome) {
            Color c = biome->getColor();
            biomeColors[i] = glm::vec4(
                c.r / 255.0f,
                c.g / 255.0f,
                c.b / 255.0f,
                1.0f
            );
        }
    }

    // -------------------------- Motor de mapas --------------------------
    std::cout << "Inicializando generador de mundo...\n";   
    LakeConfig Parametros_Lago (0.01f, -0.4f);
    WorldSystem Map_Engine(Biome_Engine, worldSeed, 128, Parametros_Lago, 8, 12);

    // -------------------------- Motor grafico --------------------------
    // Configuración gráfica
    GraphicsConfig config;
    config.windowWidth = 1600;
    config.windowHeight = 900;
    config.windowTitle = "World map";
    config.vsync = false;
    config.tileSize = 32;
    
    // Sistema de renderizado
    std::cout << "Inicializando renderizador...\n";
    RenderSystem Graphics_Engine(config, Map_Engine.GetChunkSize());
    if (!Graphics_Engine.init()) {
        std::cerr << "Error: No se pudo inicializar el sistema gráfico\n";
        return -1;
    }

    // Variables para el game loop
    auto lastTime = std::chrono::high_resolution_clock::now();
    int frameCount = 0;
    float fpsUpdateTime = 0.0f;

    std::cout << "=== EJECUTANDO LOOP PRINCIPAL ===\n";
    std::cout << "Controles:\n";
    std::cout << "  WASD - Mover cámara\n";
    std::cout << "  Rueda del ratón - Zoom\n";
    std::cout << "  ESC - Salir\n";

    // ------------------------ Debug ------------------------
    float timeSinceLastLoad = 0.0f;
    int currentRadius = 0;
    const int maxRadius = 15; // Radio máximo de chunks a cargar
    bool loadingMode = true; // true = cargando, false = descargando
    
    auto processChunk = [&](const ChunkCoord& coord, bool load) {
            if (load) {
                DynamicArray<glm::vec4> chnk_colors(Map_Engine.GetChunkSize() * Map_Engine.GetChunkSize());

                // Cargar chunk
                Map_Engine.LoadChunk(coord);
                const Chunk* chnk = Map_Engine.GetChunk(coord);
                
                if (chnk) {
                    // Generar colores
                    for (uint32_t y = 0; y < Map_Engine.GetChunkSize(); ++y) {
                        for (uint32_t x = 0; x < Map_Engine.GetChunkSize(); ++x) {
                            const Tile& tile = (*chnk)[y][x];
                            
                            int biomeId = tile.getBiomeId();
                            glm::vec4 color = biomeColors[biomeId];

                            if(tile.hasWater()) color = glm::vec4(0.0f, 0.3f, 0.8f, 0.7f);

                            chnk_colors[y * Map_Engine.GetChunkSize() + x] = color;
                        }
                    }
                    // Actualizar gráficos
                    Graphics_Engine.updateChunk(coord, chnk_colors.data());
                }
            } else {
                // Descargar chunk
                Graphics_Engine.removeChunk(coord);
                Map_Engine.UnloadChunk(coord);
            }
    };
    
    // -------------------------------------------------------
    // Game loop principal
    while (!Graphics_Engine.shouldClose()) {
        // Calcular deltaTime
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;
        
        // Actualizar estadísticas FPS
        frameCount++;
        fpsUpdateTime += deltaTime;
        if (fpsUpdateTime >= 1.0f) {
            float fps = frameCount / fpsUpdateTime;
            std::cout << "FPS: " << fps << "\n";
            frameCount = 0;
            fpsUpdateTime = 0.0f;
        }

        // Procesar entrada del usuario
        Graphics_Engine.processInput(deltaTime);

        // ------------------------ Logica del mundo -------------------------------

        timeSinceLastLoad += deltaTime;

        if (timeSinceLastLoad >= 2.0f) {
            
            if (currentRadius == 0) {
                // Chunk central
                processChunk(ChunkCoord(0, 0), loadingMode);
            } else {
                // Procesar los 4 lados del cuadrado/anillo
                
                // Lados superior e inferior
                for (int x = -currentRadius; x <= currentRadius; ++x) {
                    processChunk(ChunkCoord(x, -currentRadius), loadingMode);
                    processChunk(ChunkCoord(x, currentRadius), loadingMode);
                }
                
                // Lados izquierdo y derecho (sin incluir las esquinas ya procesadas)
                for (int y = -currentRadius + 1; y <= currentRadius - 1; ++y) {
                    processChunk(ChunkCoord(-currentRadius, y), loadingMode);
                    processChunk(ChunkCoord(currentRadius, y), loadingMode);
                }
            }

            if(loadingMode) {
                ++currentRadius;
                if(currentRadius>maxRadius) loadingMode = !loadingMode;
            
            } else {
                --currentRadius;
                if(currentRadius<0) loadingMode = !loadingMode;
            }


            timeSinceLastLoad = 0.0f;
        }

        // -------------------------------------------------------------------------

        // Renderizar el Frame
        Graphics_Engine.renderFrame();
    }

  return 0;

}