#include "graphics/Camera.hpp"

// ----- Constructores -----
Camera::Camera() : _viewportSize(0.0f, 0.0f) {
    updateMatrices();
}

Camera::Camera(int viewportWidth, int viewportHeight)    
: _viewportSize(viewportWidth, viewportHeight) {
    updateMatrices();
}

Camera::Camera(Camera&& other) noexcept 
: _position(std::move(other._position)),
  _zoom(std::move(other._zoom)),
  _viewportSize(std::move(other._viewportSize)),
  _viewMatrix(std::move(other._viewMatrix)),
  _projectionMatrix(std::move(other._projectionMatrix)),
  _viewProjectionMatrix(std::move(other._viewProjectionMatrix)),
  _dirty(std::move(other._dirty)) {}
    
// ----- Operadores -----

Camera& Camera::operator=(Camera&& other) noexcept {
    if (this != &other) {
        _position = std::move(other._position);
        _zoom = std::move(other._zoom);
        _viewportSize = std::move(other._viewportSize);
        _viewMatrix = std::move(other._viewMatrix);
        _projectionMatrix = std::move(other._projectionMatrix);
        _viewProjectionMatrix = std::move(other._viewProjectionMatrix);
        _dirty = std::move(other._dirty);
    }
    return *this;
}

// ----- Metodos publicos -----
// Getters
void Camera::setPosition(float x, float y) {
    _position.x = x;
    _position.y = y;
    _dirty = true;
}

void Camera::move(float dx, float dy) {
    _position.x += dx;
    _position.y += dy;
    _dirty = true;
}

void Camera::setZoom(float zoom) {
    _zoom = zoom;
    _dirty = true;
}

void Camera::zoom(float factor) {
    _zoom *= factor;
    _dirty = true;
}

void Camera::setViewportSize(int width, int height) {
    _viewportSize.x = width;
    _viewportSize.y = height;
    _dirty = true;
}

// Conversión de coordenadas
glm::vec2 Camera::screenToWorld(float screenX, float screenY) const {
    // Convertir de pantalla a mundo
    float worldX = (screenX - _viewportSize.x / 2.0f) / _zoom + _position.x;
    float worldY = (screenY - _viewportSize.y / 2.0f) / _zoom + _position.y;
    return {worldX, worldY};
}

glm::vec2 Camera::worldToScreen(float worldX, float worldY) const {
    // Convertir de mundo a pantalla
    float screenX = (worldX - _position.x) * _zoom + _viewportSize.x / 2.0f;
    float screenY = (worldY - _position.y) * _zoom + _viewportSize.y / 2.0f;
    return {screenX, screenY};
}

void Camera::update() {
    if (_dirty) {
        updateMatrices();
        _dirty = false;
    }
}

void Camera::reset() {
    _position = {0.0f, 0.0f};
    _zoom = 1.0f;
    _dirty = true;
}

// ----- Metodos privados -----

void Camera::updateMatrices() {
    // Matriz de proyección ortográfica 2D
    float halfWidth = _viewportSize.x / (2.0f * _zoom);
    float halfHeight = _viewportSize.y / (2.0f * _zoom);
    
    _projectionMatrix = glm::ortho(
        -halfWidth, halfWidth,
        -halfHeight, halfHeight,
        -1.0f, 1.0f
    );
    
    // Matriz de vista (cámara)
    _viewMatrix = glm::translate(glm::mat4(1.0f), 
        glm::vec3(-_position.x, -_position.y, 0.0f));
    
    // Matriz combinada
    _viewProjectionMatrix = _projectionMatrix * _viewMatrix;
}
