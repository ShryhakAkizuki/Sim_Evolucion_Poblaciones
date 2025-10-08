#pragma once

#include <SFML/Graphics.hpp>
#include "Mapa.hpp"

class Renderer {
private:
    sf::RenderWindow ventana;
    int pixel_celda_min;
    
    void dibujarMapa(const Mapa& mapa);

public:
    Renderer(int anchoVentana, int altoVentana, int pixel_celda_min);
    void ejecutar(const Mapa& mapa); // Bucle principal de render

};