#include "mapa.hpp"
#include "estructuras.hpp"
#include <iostream>

const int NUM_BIOMAS = 5;

int main(){
    
  // Crear mapa de 10x10
  Mapa mapa(0.05, 0.05,1);

  Bioma Biomas[NUM_BIOMAS] = {
    Bioma(1, "Bosque", 20.0f, 80.0f, 0.1f, 1.0f, 0.05f, 0.02f, 2.0f, 5.0f, 0.01f, 0.1f, 0.01f, 0.01f),
    Bioma(2, "Desierto", 35.0f, 20.0f, 0.5f, 1.5f, 0.02f, 0.01f, 3.0f, 5.0f, 0.05f, 0.1f, 0.01f, 0.01f),
    Bioma(3, "Pradera", 25.0f, 60.0f, 0.2f, 1.2f, 0.03f, 0.02f, 2.0f, 4.0f, 0.02f, 0.05f, 0.01f, 0.01f),
    Bioma(4, "Tundra", 0.0f, 50.0f, 0.1f, 1.0f, 0.01f, 0.02f, 1.0f, 3.0f, 0.01f, 0.03f, 0.01f, 0.01f),
    Bioma(5, "Selva", 28.0f, 90.0f, 0.3f, 1.2f, 0.04f, 0.03f, 3.0f, 5.0f, 0.03f, 0.06f, 0.01f, 0.01f)
  };


  mapa.DistribuirBiomas_Voronoi(Biomas, NUM_BIOMAS, 3);
  mapa.generarCuerposDeAgua(0.4, 4, 8, 3, 6);
  std::cout << mapa;

  return 0;

}