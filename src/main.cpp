#define NOMINMAX

#include "map/ChunkManager.hpp"
#include "map/WorldGenerator.hpp"
#include "biome/BiomeSystem.hpp"
#include "biome/Biome.hpp"
// #include "visualization/GameWindow.hpp"

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

    // Definir Biomas:
    auto biomeSystem = std::make_shared<BiomeSystem>(worldSeed);
    
    biomeSystem->registrarBioma(std::make_shared<Bioma>(
        0, "Llanura", Color{124, 252, 0},
        22.0f, 45.0f, 70.0f, 1.0f, 0.2f, 0.2f,
        6.0f, 15.0f, 12.0f, 0.15f, 0.08f, 0.07f,
        static_cast<uint32_t>(worldSeed)
    ));
    
    biomeSystem->registrarBioma(std::make_shared<Bioma>(
        1, "Bosque", Color{34, 139, 34},
        18.0f, 65.0f, 50.0f, 1.3f, 0.15f, 0.1f,
        4.0f, 12.0f, 10.0f, 0.25f, 0.06f, 0.04f,
        static_cast<uint32_t>(worldSeed)
    ));
    
    biomeSystem->registrarBioma(std::make_shared<Bioma>(
        2, "Colinas", Color{139, 115, 85},
        16.0f, 55.0f, 65.0f, 1.4f, 0.1f, 0.15f,
        5.0f, 10.0f, 8.0f, 0.3f, 0.05f, 0.06f,
        static_cast<uint32_t>(worldSeed)
    ));
    
    biomeSystem->registrarBioma(std::make_shared<Bioma>(
        3, "Pantano", Color{48, 128, 20},
        20.0f, 85.0f, 45.0f, 1.6f, 0.25f, 0.05f,
        3.0f, 8.0f, 6.0f, 0.4f, 0.12f, 0.02f,
        static_cast<uint32_t>(worldSeed)
    ));
    
    biomeSystem->registrarBioma(std::make_shared<Bioma>(
        4, "Desierto", Color{255, 230, 150},
        35.0f, 15.0f, 85.0f, 1.5f, 0.05f, 0.3f,
        8.0f, 5.0f, 10.0f, 0.25f, 0.03f, 0.12f,
        static_cast<uint32_t>(worldSeed)
    ));
    
    biomeSystem->registrarBioma(std::make_shared<Bioma>(
        5, "Sabana", Color{210, 180, 140},
        28.0f, 35.0f, 75.0f, 1.2f, 0.12f, 0.18f,
        7.0f, 10.0f, 9.0f, 0.2f, 0.07f, 0.08f,
        static_cast<uint32_t>(worldSeed)
    ));
    
    biomeSystem->registrarBioma(std::make_shared<Bioma>(
        6, "Tundra", Color{200, 220, 240},
        5.0f, 60.0f, 40.0f, 1.3f, 0.08f, 0.12f,
        4.0f, 12.0f, 7.0f, 0.25f, 0.04f, 0.05f,
        static_cast<uint32_t>(worldSeed)
    ));

    // Definir Generador procedural:

    LakeConfig Parametros_Lago (0.01f, -0.4f);
    auto worldGenerator = std::make_shared<WorldGenerator>(worldSeed, biomeSystem, Parametros_Lago);

    // // Definir Gestor de Chunks:
    // ChunkManager chunkManager(16, worldGenerator, 8, 12, 1600);

    // chunkManager.generarRegionInicial(100,100,100);

    // std::cout << "=== SISTEMA DE VISUALIZACION CON SFML ===\n";

    // std::cout << "Iniciando visualizacion SFML...\n";
    // std::cout << "   Controles:\n";
    // std::cout << "   - Click izquierdo + arrastrar: Mover camara\n";
    // std::cout << "   - Rueda del mouse: Zoom in/out\n";
    // std::cout << "   - Click derecho: Informacion del tile\n";
    
    // GameWindow gameWindow(chunkManager, 1920, 1080);
    // gameWindow.run();


  return 0;

}