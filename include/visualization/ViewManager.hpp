#pragma once
#include <SFML/Graphics.hpp>

class ViewManager {
private:
    sf::View _uiView;
    sf::View _gameView;
    
    sf::Vector2u _windowSize;
    
public:
    ViewManager(const sf::Vector2u& windowSize);
    
    void resize(const sf::Vector2u& newSize);
    
    void setGameView(const sf::View& view);
    const sf::View& getGameView() const { return _gameView; }
    const sf::View& getUIView() const { return _uiView; }
    
    void applyGameView(sf::RenderTarget& target) const;
    void applyUIView(sf::RenderTarget& target) const;
};