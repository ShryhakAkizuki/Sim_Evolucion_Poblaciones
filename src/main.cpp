#define NOMINMAX
//#include "mapa.hpp"
//#include "estructuras.hpp"
//#include "Renderer.hpp"
#include "map/ChunkManager.hpp"
#include "visualization/GameWindow.hpp"

#include <iostream>

const int NUM_BIOMAS = 5;

int main(){

    std::cout << "=== SISTEMA DE VISUALIZACIÓN CON SFML ===\n";
    
    try {
        // Crear ChunkManager
        std::cout << "1. Inicializando ChunkManager...\n";
        ChunkManager chunkManager(16, 8, 12, 1000, 123456);
        
        // Generar región inicial
        // std::cout << "2. Generando región inicial...\n";
        // chunkManager.generarRegionInicial(0, 0, 3);
        
        std::cout << "3. Chunks cargados: " << chunkManager.GetLoadedChunkCount() << "\n";
        
        // Crear y ejecutar ventana
        std::cout << "4. Iniciando visualización SFML...\n";
        std::cout << "   Controles:\n";
        std::cout << "   - Click izquierdo + arrastrar: Mover cámara\n";
        std::cout << "   - Rueda del mouse: Zoom in/out\n";
        std::cout << "   - Click derecho: Información del tile\n";
        
        GameWindow gameWindow(chunkManager, 1200, 800);
        gameWindow.run();
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
    
    std::cout << "Aplicación terminada correctamente\n";











    // std::cout << "=== SISTEMA DE GENERACIÓN PROCEDURAL INTEGRADO ===\n";
    
    // // Opción 1: Crear con semilla automática
    // std::cout << "\n1. Creando ChunkManager con semilla 12345...\n";
    // ChunkManager chunkManager(16, 8, 12, 10000, 12345);
    
    // // Opción 2: Crear con WorldGenerator personalizado
    // /*
    // auto worldGenerator = std::make_shared<WorldGenerator>(12345);
    // worldGenerator->setRiverFrequency(0.1f);
    // ChunkManager chunkManager(16, worldGenerator, 8, 12, 1000);
    // */
    
    // // Generar región inicial
    // std::cout << "\n2. Generando región inicial...\n";
    // chunkManager.generarRegionInicial(0, 0, 3); // 7x7 chunks alrededor del origen
    
    // // Acceder a chunks generados
    // std::cout << "\n3. Accediendo a chunks generados...\n";
    // auto chunk = chunkManager.GetChunk(ChunkCoord{1, 1});
    
    // // Ver contenido generado
    // std::cout << "\n4. Verificando contenido procedural...\n";
    // for (int worldX = 20; worldX < 25; worldX++) {
    //     for (int worldY = 20; worldY < 25; worldY++) {
    //         const Tile& tile = chunkManager.GetTile(worldX, worldY);
    //         std::cout << "Tile(" << worldX << "," << worldY << "): "
    //                   << "Bioma=" << tile.getBiomeId()
    //                   << ", Agua=" << tile.hasWater()
    //                   << "\n";
    //     }
    // }
    
    // // Simular ciclo día/noche
    // std::cout << "\n5. Simulando ciclo día/noche...\n";
    // for (float hora = 6.0f; hora <= 18.0f; hora += 3.0f) {
    //     chunkManager.actualizarEstadosBiomas(hora);
    //     std::cout << "Hora " << hora << ":00 - Biomas actualizados\n";
    // }
    
    // // Expansión dinámica
    // std::cout << "\n6. Expansión dinámica a chunk lejano...\n";
    // auto chunkLejano = chunkManager.GetChunk(ChunkCoord{35, 35});
    // std::cout << "Chunk (35,35) cargado: " 
    //           << (chunkLejano ? "ÉXITO" : "FALLÓ") << "\n";
    
    // std::cout << "\n7. Estadísticas finales:\n";
    // std::cout << "Chunks cargados: " << chunkManager.GetLoadedChunkCount() << "\n";
    
























  // char buffer[MAX_PATH];
  // GetModuleFileNameA(NULL, buffer, MAX_PATH);
  // std::filesystem::path exePath(buffer);
  // std::filesystem::path exeDir = exePath.parent_path();
  // std::filesystem::path basePath = exeDir / "mapa";

  // Dynamic_Map<int>mapa(4,2,4,basePath.string());
  // mapa.setCell(0,0,87);
  // mapa.setCell(2*4,0,87);
  // mapa.setCell(2*4,6*4,87);
  // std::cout<<mapa<<std::endl;

  // mapa.unloadChunk(0,0,0);
  // mapa.unloadChunk(2,0,0);
  // mapa.unloadChunk(2,6,0);
  // std::cout<<mapa<<std::endl;

  // mapa.loadChunk(0,0,0);
  // mapa.loadChunk(2,0,0);
  // mapa.loadChunk(2,6,0);
  // std::cout<<mapa<<std::endl;





  // Chunk<int>* chunk = mapa.generateNewChunk(0,0,0);
  // for(int i=0;i<chunk->sizeY;i++){
  //     for(int j=0;j<chunk->sizeX;j++){
  //         chunk->cells[i][j].data = i*chunk->sizeX + j;
  //         std::cout << chunk->cells[i][j].data << " ";
  //     }
  //     std::cout << std::endl;
  // }

  // std::cout<<chunk<<std::endl;

  // // Crear mapa de 10x10
  // Mapa mapa(1, 1,1);

  // Bioma Biomas[NUM_BIOMAS] = {
  //   Bioma(1, "Bosque", Color{34, 139, 34}, 20.0f, 80.0f, 0.1f, 1.0f, 0.05f, 0.02f, 2.0f, 5.0f, 0.01f, 0.1f, 0.01f, 0.01f),
  //   Bioma(2, "Desierto", Color{237, 201, 175}, 35.0f, 20.0f, 0.5f, 1.5f, 0.02f, 0.01f, 3.0f, 5.0f, 0.05f, 0.1f, 0.01f, 0.01f),
  //   Bioma(3, "Pradera", Color{124, 252, 0}, 25.0f, 60.0f, 0.2f, 1.2f, 0.03f, 0.02f, 2.0f, 4.0f, 0.02f, 0.05f, 0.01f, 0.01f),
  //   Bioma(4, "Tundra", Color{176, 196, 222}, 0.0f, 50.0f, 0.1f, 1.0f, 0.01f, 0.02f, 1.0f, 3.0f, 0.01f, 0.03f, 0.01f, 0.01f),
  //   Bioma(5, "Selva", Color{0, 100, 0}, 28.0f, 90.0f, 0.3f, 1.2f, 0.04f, 0.03f, 3.0f, 5.0f, 0.03f, 0.06f, 0.01f, 0.01f)
  // };

  // mapa.DistribuirBiomas_Voronoi(Biomas, NUM_BIOMAS, 6);
  // mapa.generarCuerposDeAgua(0.25, 4, 4, 30, 100, 8);
  // //std::cout << mapa;

  // Renderer visualizacion(1920,1080,1);
  // visualizacion.ejecutar(mapa);

  return 0;

}