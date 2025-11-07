#include "visualization/ViewManager.hpp"
#include <iostream>

ViewManager::ViewManager(const sf::Vector2u& windowSize) 
    : _windowSize(windowSize) {
    
    // Vista para la UI (coordenadas de píxeles)
    _uiView = sf::View(sf::FloatRect(0, 0, windowSize.x, windowSize.y));
    
    // Vista para el juego (coordenadas del mundo)
    _gameView = sf::View(sf::FloatRect(0, 0, windowSize.x, windowSize.y));
    
    std::cout << "ViewManager inicializado con tamaño de ventana: " 
              << windowSize.x << "x" << windowSize.y << "\n";
}

void ViewManager::resize(const sf::Vector2u& newSize) {
    _windowSize = newSize;
    
    // Actualizar vista de UI
    _uiView.setSize(newSize.x, newSize.y);
    _uiView.setCenter(newSize.x / 2.0f, newSize.y / 2.0f);
    
    // La vista del juego se actualiza externamente por CameraController
}

void ViewManager::setGameView(const sf::View& view) {
    _gameView = view;
}

void ViewManager::applyGameView(sf::RenderTarget& target) const {
    target.setView(_gameView);
}

void ViewManager::applyUIView(sf::RenderTarget& target) const {
    target.setView(_uiView);
}