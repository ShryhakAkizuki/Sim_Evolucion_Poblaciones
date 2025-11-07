#include "Renderer.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>

Renderer::Renderer(int anchoVentana, int altoVentana, int pixel_celda_min)
: ventana(sf::VideoMode(anchoVentana, altoVentana), "Simulador de Ecosistema"), pixel_celda_min(pixel_celda_min){
    
    ventana.setFramerateLimit(60);
}

void Renderer::ejecutar(const Mapa& mapa) {
    while (ventana.isOpen()) {
        sf::Event evento;
        while (ventana.pollEvent(evento)) {
            if (evento.type == sf::Event::Closed)
                ventana.close();
        }

        dibujarMapa(mapa);
    }
}

void Renderer::dibujarMapa(const Mapa& mapa){
    ventana.clear();

    for(int y=0; y<mapa.getMapaCeldas_y(); ++y){
        for(int x=0; x<mapa.getMapaCeldas_x(); ++x){
            const CeldaBase& celda = mapa.getCelda(x,y);

            sf::RectangleShape rect(sf::Vector2f(pixel_celda_min, pixel_celda_min));
            rect.setPosition(x * pixel_celda_min, y * pixel_celda_min);

            if (celda.tiene_agua) {
                rect.setFillColor(sf::Color(28, 107, 160)); // azul agua
            } else {
                const Color& c = celda.bioma->getBiomaColor();
                rect.setFillColor(sf::Color(c.r, c.g, c.b));
            }

            ventana.draw(rect);
        }
    }
    ventana.display();
}