#include <iostream>

#include "graphics/InputManager.hpp"

// ----- Constructores -----
InputManager::InputManager(GLFWwindow* window) : _window(window) {
    _keyStates = Unordered_map<int, KeyState>(32);
    _mouseButtonStates = Unordered_map<int, KeyState>(5);
}

InputManager::InputManager(InputManager&& other) noexcept 
: _window(other._window),
  _keyStates(std::move(other._keyStates)),
  _mouseButtonStates(std::move(other._mouseButtonStates)),
  _mousePosition(std::move(other._mousePosition)),
  _lastMousePosition(std::move(other._lastMousePosition)),
  _mouseDelta(std::move(other._mouseDelta)),
  _mouseScroll(std::move(other._mouseScroll)) {
    other._window = nullptr;
}
// ----- Operadores -----

InputManager& InputManager::operator=(InputManager&& other) noexcept {
    if (this != &other) {
        _window = other._window;
        other._window = nullptr;

        _keyStates = std::move(other._keyStates);
        _mouseButtonStates = std::move(other._mouseButtonStates);
        _mousePosition = std::move(other._mousePosition);
        _lastMousePosition = std::move(other._lastMousePosition);
        _mouseDelta = std::move(other._mouseDelta);
        _mouseScroll = std::move(other._mouseScroll);
    }
    return *this;
}


// ----- Métodos -----
// Actualización del estado
void InputManager::update() {
    // Actualizar estados anteriores
    for (auto it = _keyStates.begin(); it != _keyStates.end(); ++it) {
        it->second().previous = it->second().current;
    }
    
    for (auto it = _mouseButtonStates.begin(); it != _mouseButtonStates.end(); ++it) {
        it->second().previous = it->second().current;
    }
    
    // Calcular delta del ratón
    _mouseDelta.x = _mousePosition.x - _lastMousePosition.x;
    _mouseDelta.y = _mousePosition.y - _lastMousePosition.y;
    _lastMousePosition = _mousePosition;
}

void InputManager::processKeyEvent(int key, int action) {
    KeyState& state = _keyStates[key];
    
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        state.current = true;
    } else if (action == GLFW_RELEASE) {
        state.current = false;
    }
}

void InputManager::processMouseButtonEvent(int button, int action) {
    KeyState& state = _mouseButtonStates[button];
    
    if (action == GLFW_PRESS) {
        state.current = true;
    } else if (action == GLFW_RELEASE) {
        state.current = false;
    }
}

void InputManager::processMouseMoveEvent(double x, double y) {
    _mousePosition.x = static_cast<float>(x);
    _mousePosition.y = static_cast<float>(y);
}

void InputManager::processMouseScrollEvent(double xoffset, double yoffset) {
    _mouseScroll += static_cast<float>(yoffset);
}

//Consultas de estado

bool InputManager::isKeyDown(int key) const {
    const KeyState* state = _keyStates.find_ptr(key);
    return state ? state->isDown() : false;
}

bool InputManager::isKeyPressed(int key) const {
    const KeyState* state = _keyStates.find_ptr(key);
    return state ? state->isPressed() : false;
}

bool InputManager::isKeyReleased(int key) const {
    const KeyState* state = _keyStates.find_ptr(key);
    return state ? state->isReleased() : false;
}

bool InputManager::isMouseButtonDown(int button) const {
    const KeyState* state = _mouseButtonStates.find_ptr(button);
    return state ? state->isDown() : false;
}

bool InputManager::isMouseButtonPressed(int button) const {
    const KeyState* state = _mouseButtonStates.find_ptr(button);
    return state ? state->isPressed() : false;
}

bool InputManager::isMouseButtonReleased(int button) const {
    const KeyState* state = _mouseButtonStates.find_ptr(button);
    return state ? state->isReleased() : false;
}

// Getters

float InputManager::getMouseScroll() {
    float scroll = _mouseScroll;
    _mouseScroll = 0.0f;
    return scroll;
}