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
        0, "Llanura",
        22.0f, 45.0f, 70.0f, 1.0f, 0.2f, 0.2f,
        6.0f, 15.0f, 12.0f, 0.15f, 0.08f, 0.07f,
        static_cast<uint32_t>(worldSeed)
    ));
    
    Biome_Engine.registrarBioma(std::make_unique<Bioma>(
        1, "Bosque",
        18.0f, 65.0f, 50.0f, 1.3f, 0.15f, 0.1f,
        4.0f, 12.0f, 10.0f, 0.25f, 0.06f, 0.04f,
        static_cast<uint32_t>(worldSeed)
    ));
    
    Biome_Engine.registrarBioma(std::make_unique<Bioma>(
        2, "Colinas",
        16.0f, 55.0f, 65.0f, 1.4f, 0.1f, 0.15f,
        5.0f, 10.0f, 8.0f, 0.3f, 0.05f, 0.06f,
        static_cast<uint32_t>(worldSeed)
    ));
    
    Biome_Engine.registrarBioma(std::make_unique<Bioma>(
        3, "Pantano",
        20.0f, 85.0f, 45.0f, 1.6f, 0.25f, 0.05f,
        3.0f, 8.0f, 6.0f, 0.4f, 0.12f, 0.02f,
        static_cast<uint32_t>(worldSeed)
    ));

    
    Biome_Engine.registrarBioma(std::make_unique<Bioma>(
        4, "Desierto",
        35.0f, 15.0f, 85.0f, 1.5f, 0.05f, 0.3f,
        8.0f, 5.0f, 10.0f, 0.25f, 0.03f, 0.12f,
        static_cast<uint32_t>(worldSeed)
    ));
    
    Biome_Engine.registrarBioma(std::make_unique<Bioma>(
        5, "Sabana",
        28.0f, 35.0f, 75.0f, 1.2f, 0.12f, 0.18f,
        7.0f, 10.0f, 9.0f, 0.2f, 0.07f, 0.08f,
        static_cast<uint32_t>(worldSeed)
    ));
    
    Biome_Engine.registrarBioma(std::make_unique<Bioma>(
        6, "Tundra",
        5.0f, 60.0f, 40.0f, 1.3f, 0.08f, 0.12f,
        4.0f, 12.0f, 7.0f, 0.25f, 0.04f, 0.05f,
        static_cast<uint32_t>(worldSeed)
    ));

    DynamicArray<glm::vec4> biomeColors = {
        // Llanura
        glm::vec4(124.0f/255.0f, 252.0f/255.0f, 0.0f/255.0f, 1.0f),
        // Bosque
        glm::vec4(34.0f/255.0f, 139.0f/255.0f, 34.0f/255.0f, 1.0f),
        // Colinas
        glm::vec4(139.0f/255.0f, 115.0f/255.0f, 85.0f/255.0f, 1.0f),
        // Pantano
        glm::vec4(48.0f/255.0f, 128.0f/255.0f, 20.0f/255.0f, 1.0f),
        // Desierto
        glm::vec4(255.0f/255.0f, 230.0f/255.0f, 150.0f/255.0f, 1.0f),
        // Sabana
        glm::vec4(210.0f/255.0f, 180.0f/255.0f, 140.0f/255.0f, 1.0f),
        // Tundra
        glm::vec4(200.0f/255.0f, 220.0f/255.0f, 240.0f/255.0f, 1.0f),
    };

    // -------------------------- Motor de mapas --------------------------
    std::cout << "Inicializando generador de mundo...\n";   
    LakeConfig Parametros_Lago (0.01f, -0.4f);
    WorldSystem Map_Engine(Biome_Engine.getTodosBiomasID(), worldSeed, 128, Parametros_Lago, 6, 8, 500,1);

    // -------------------------- Motor grafico --------------------------
    // Configuración gráfica
    GraphicsConfig config;
    config.windowWidth = 1600;
    config.windowHeight = 900;
    config.windowTitle = "World map";
    config.vsync = true;
    config.tileSize = 2;
    
    // Sistema de renderizado
    std::cout << "Inicializando renderizador...\n";
    RenderSystem Graphics_Engine(config, biomeColors, Map_Engine.GetChunkSize());
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
    float timeSinceLastLoad = 60.0f;
    const int maxRadius = 15; // Radio máximo de chunks a cargar
    bool loadingMode = true; // true = cargando, false = descargando
    DynamicArray<ChunkCoord> Debug_Coords;

    for(int currentRadius = 0; currentRadius<=maxRadius; ++currentRadius){
        if (currentRadius == 0) {
            Debug_Coords.push_back(ChunkCoord(0, 0));

        } else {

            for (int x = -currentRadius; x <= currentRadius; ++x) {
                Debug_Coords.push_back(ChunkCoord(x, -currentRadius));
                Debug_Coords.push_back(ChunkCoord(x, currentRadius));
            }
            
            // Lados izquierdo y derecho (sin incluir las esquinas ya procesadas)
            for (int y = -currentRadius + 1; y <= currentRadius - 1; ++y) {
                Debug_Coords.push_back(ChunkCoord(-currentRadius, y));
                Debug_Coords.push_back(ChunkCoord(currentRadius, y));

            }
        }
    }    

    auto processChunk = [&](const ChunkCoord& coord, bool load) {
        if (load) Graphics_Engine.updateChunk(coord, Map_Engine.LoadChunk(coord));
        else {
            // Descargar chunk
            Graphics_Engine.removeChunk(coord);
            Map_Engine.UnloadChunk(coord);
        }
    };

    auto processChunk_Stream = [&](const DynamicArray<ChunkCoord>& coords, bool load) {
        if (load) Graphics_Engine.updateChunk(coords, Map_Engine.loadAllChunksInVector(coords));
        else {
            // Descargar chunk
            Graphics_Engine.removeChunk(coords);
            Map_Engine.UnloadAllChunksInVector(coords);
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

        if (timeSinceLastLoad >= 60.0f){
            processChunk_Stream(Debug_Coords,loadingMode);

            loadingMode = !loadingMode;        
            timeSinceLastLoad = 0;
        }
        // -------------------------------------------------------------------------

        // Renderizar el Frame
        Graphics_Engine.renderFrame();
    }

  return 0;

}