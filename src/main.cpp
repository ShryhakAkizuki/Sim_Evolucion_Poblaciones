//#include "mapa.hpp"
#include "estructuras.hpp"
//#include "Renderer.hpp"
#include <iostream>
#include <filesystem>
#include <string>
#include <windows.h>

const int NUM_BIOMAS = 5;

int main(){

  char buffer[MAX_PATH];
  GetModuleFileNameA(NULL, buffer, MAX_PATH);
  std::filesystem::path exePath(buffer);
  std::filesystem::path exeDir = exePath.parent_path();
  std::filesystem::path basePath = exeDir / "mapa";

  Dynamic_Map<int>mapa(4,2,4,basePath.string());
  mapa.setCell(0,0,87);
  mapa.setCell(2*4,0,87);
  mapa.setCell(2*4,6*4,87);
  std::cout<<mapa<<std::endl;

  mapa.unloadChunk(0,0,0);
  mapa.unloadChunk(2,0,0);
  mapa.unloadChunk(2,6,0);
  std::cout<<mapa<<std::endl;

  mapa.loadChunk(0,0,0);
  mapa.loadChunk(2,0,0);
  mapa.loadChunk(2,6,0);
  std::cout<<mapa<<std::endl;





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