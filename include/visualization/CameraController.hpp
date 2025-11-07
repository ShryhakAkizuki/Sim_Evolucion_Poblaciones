#pragma once
#include <SFML/Graphics.hpp>

class CameraController {
private:
    sf::View _cameraView;
    sf::Vector2f _cameraPosition;
    float _zoomLevel;
    float _minZoom;
    float _maxZoom;
    
    // Límites del mundo
    sf::FloatRect _worldBounds;
    
    void enforceBounds();

public:
    CameraController(const sf::Vector2f& initialPosition, const sf::FloatRect& worldBounds);
    
    void move(const sf::Vector2f& offset);
    void setPosition(const sf::Vector2f& position);
    void zoom(float factor);
    void resetZoom();
    
    void setViewSize(const sf::Vector2f& size);
        
    // Conversiones de coordenadas
    sf::Vector2f screenToWorld(const sf::Vector2i& screenPos, const sf::RenderWindow& window) const;
    sf::Vector2i worldToScreen(const sf::Vector2f& worldPos, const sf::RenderWindow& window) const;
    
    // Getters
    const sf::View& getView() const { return _cameraView; }
    sf::Vector2f getPosition() const { return _cameraPosition; }
    float getZoom() const { return _zoomLevel; }
    
    // Viewport queries
    sf::FloatRect getVisibleWorldBounds() const;
    bool isInView(const sf::FloatRect& bounds) const;
};