#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "data_structures/Unordered_map.hpp"

// ----- Structs para estado -----
struct KeyState {
    bool current = false;
    bool previous = false;
        
    bool isPressed() const { return current && !previous; }
    bool isReleased() const { return !current && previous; }
    bool isDown() const { return current; }
};

struct MousePosition {
    float x = 0.0f;
    float y = 0.0f;
    
    bool operator!=(const MousePosition& other) const {
        return x != other.x || y != other.y;
    }
};
    
// ----- InputManager usando tus estructuras -----
class InputManager {
private:
    // ----- Atributos -----
    GLFWwindow* _window;
    
    // Estados usando tus estructuras
    Unordered_map<int, KeyState> _keyStates;
    Unordered_map<int, KeyState> _mouseButtonStates;
    
    // Posición y desplazamiento del ratón
    MousePosition _mousePosition;
    MousePosition _lastMousePosition;
    MousePosition _mouseDelta;
    float _mouseScroll = 0.0f;

public:
    // ----- Constructores -----
    InputManager() = default;
    InputManager(GLFWwindow* window);
    
    InputManager(const InputManager& other) = delete;
    InputManager(InputManager&& other) noexcept;

    // ----- Operadores -----
    InputManager& operator=(const InputManager& other) = delete;
    InputManager& operator=(InputManager&& other) noexcept;

    // ----- Destructor -----
    ~InputManager() = default;

    // ----- Métodos -----
    // Actualización del estado
    void update();
    
    // Procesamiento de eventos (llamados desde callbacks de GLFW)
    void processKeyEvent(int key, int action);
    void processMouseButtonEvent(int button, int action);
    void processMouseMoveEvent(double x, double y);
    void processMouseScrollEvent(double xoffset, double yoffset);
    
    // Consultas de estado del teclado
    bool isKeyDown(int key) const;
    bool isKeyPressed(int key) const;
    bool isKeyReleased(int key) const;
    
    bool isMouseButtonDown(int button) const;
    bool isMouseButtonPressed(int button) const;
    bool isMouseButtonReleased(int button) const;

    // Getters
    const MousePosition& getMousePosition() const { return _mousePosition; }
    const MousePosition& getMouseDelta() const { return _mouseDelta; }
    float getMouseScroll();
};
