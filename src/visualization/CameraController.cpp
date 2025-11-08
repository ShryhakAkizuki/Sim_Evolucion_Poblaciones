#include "visualization/CameraController.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>

CameraController::CameraController(const sf::Vector2f& initialPosition, const sf::FloatRect& worldBounds)
    : _cameraPosition(initialPosition), _zoomLevel(1.0f), 
      _minZoom(0.1f), _maxZoom(5.0f), _worldBounds(worldBounds) {
    
    _cameraView.setCenter(_cameraPosition);
    _cameraView.setSize(800, 600);
    std::cout << "CameraController inicializado\n";
}

void CameraController::move(const sf::Vector2f& offset) {
    _cameraPosition += offset;
    enforceBounds();
    _cameraView.setCenter(_cameraPosition);
}

void CameraController::setPosition(const sf::Vector2f& position) {
    _cameraPosition = position;
    enforceBounds();
    _cameraView.setCenter(_cameraPosition);
}

void CameraController::zoom(float factor) {
    _zoomLevel *= factor;
    _zoomLevel = std::clamp(_zoomLevel, _minZoom, _maxZoom);
}

void CameraController::resetZoom() {
    _zoomLevel = 1.0f;
}

void CameraController::setViewSize(const sf::Vector2f& size) {
    _cameraView.setSize(size.x / _zoomLevel, size.y / _zoomLevel);
    _cameraView.setCenter(_cameraPosition);
    enforceBounds();
}

void CameraController::enforceBounds() {
    sf::Vector2f viewSize = _cameraView.getSize();
    
    float left = _cameraPosition.x - viewSize.x / 2.0f;
    float top = _cameraPosition.y - viewSize.y / 2.0f;
    
    if (left < _worldBounds.left) {
        _cameraPosition.x = _worldBounds.left + viewSize.x / 2.0f;
    }
    if (left + viewSize.x > _worldBounds.left + _worldBounds.width) {
        _cameraPosition.x = (_worldBounds.left + _worldBounds.width) - viewSize.x / 2.0f;
    }
    if (top < _worldBounds.top) {
        _cameraPosition.y = _worldBounds.top + viewSize.y / 2.0f;
    }
    if (top + viewSize.y > _worldBounds.top + _worldBounds.height) {
        _cameraPosition.y = (_worldBounds.top + _worldBounds.height) - viewSize.y / 2.0f;
    }
}

sf::Vector2f CameraController::screenToWorld(const sf::Vector2i& screenPos, const sf::RenderWindow& window) const {
    return window.mapPixelToCoords(screenPos, _cameraView);
}

sf::Vector2i CameraController::worldToScreen(const sf::Vector2f& worldPos, const sf::RenderWindow& window) const {
    return window.mapCoordsToPixel(worldPos, _cameraView);
}

sf::FloatRect CameraController::getVisibleWorldBounds() const {
    sf::Vector2f viewSize = _cameraView.getSize();
    sf::Vector2f viewCenter = _cameraView.getCenter();
    
    return sf::FloatRect(
        viewCenter.x - viewSize.x / 2.0f,
        viewCenter.y - viewSize.y / 2.0f,
        viewSize.x,
        viewSize.y
    );
}

bool CameraController::isInView(const sf::FloatRect& bounds) const {
    sf::FloatRect visibleBounds = getVisibleWorldBounds();
    return visibleBounds.intersects(bounds);
}