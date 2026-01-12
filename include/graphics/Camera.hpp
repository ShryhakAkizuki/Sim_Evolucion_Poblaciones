#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
private:
    // ----- Atributos -----
    glm::vec2 _position = {0.0f, 0.0f};
    float _zoom = 0.05f;
    glm::vec2 _viewportSize;
    
    glm::mat4 _viewMatrix;
    glm::mat4 _projectionMatrix;
    glm::mat4 _viewProjectionMatrix;
    bool _dirty = true;

public:
    // ----- Constructores -----
    Camera();
    Camera(int viewportWidth, int viewportHeight);

    Camera(const Camera& other) = delete;
    Camera(Camera&& other) noexcept;

    // ----- Destructor -----
    ~Camera() = default;
    
    // ----- Operadores -----
    Camera& operator=(const Camera& other) = delete;
    Camera& operator=(Camera&& other) noexcept;

    // ----- Métodos -----
    // Getters
    const glm::vec2& getPosition() const { return _position; }
    const float& getZoom() const { return _zoom; }
    const glm::vec2& getViewportSize() const { return _viewportSize; }
    float getAdjustedSpeed(float baseSpeed) const { return baseSpeed / _zoom; }
    
    // Transformaciones
    void setPosition(float x, float y);
    void move(float dx, float dy);
    void setZoom(float zoom);
    void zoom(float factor);
    void setViewportSize(int width, int height);
    
    // Matrices
    const glm::mat4& getViewMatrix() const { return _viewMatrix; }
    const glm::mat4& getProjectionMatrix() const { return _projectionMatrix; }
    const glm::mat4& getViewProjectionMatrix() const { return _viewProjectionMatrix; }
    
    // Conversión de coordenadas
    glm::vec2 screenToWorld(float screenX, float screenY) const;
    glm::vec2 worldToScreen(float worldX, float worldY) const;
    
    // Actualización
    void update();
    void reset();
    
private:
    void updateMatrices();

};