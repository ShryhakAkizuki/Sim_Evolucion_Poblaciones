#include "visualization/InputHandler.hpp"
#include <iostream>

InputHandler::InputHandler()
    : _currentState(InputState::IDLE), _mouseState{} {
    
    _mouseState.leftPressed = false;
    _mouseState.rightPressed = false;
    _mouseState.middlePressed = false;
    
    std::cout << "InputHandler inicializado\n";
}

void InputHandler::handleEvent(const sf::Event& event, 
                              const sf::RenderWindow& window,
                              const CameraController& camera) {
    switch (event.type) {
        case sf::Event::MouseButtonPressed:
            handleMousePress(event.mouseButton, window, camera);
            break;
            
        case sf::Event::MouseButtonReleased:
            handleMouseRelease(event.mouseButton);
            break;
            
        case sf::Event::MouseMoved:
            handleMouseMove(event.mouseMove, window, camera);
            break;
            
        case sf::Event::MouseWheelScrolled:
            handleMouseWheel(event.mouseWheelScroll);
            break;
            
        default:
            break;
    }
}

void InputHandler::handleMousePress(const sf::Event::MouseButtonEvent& mouseEvent, 
                                   const sf::RenderWindow& window, 
                                   const CameraController& camera) {
    _mouseState.position = sf::Vector2i(mouseEvent.x, mouseEvent.y);
    _mouseState.lastPosition = _mouseState.position;
    
    if (mouseEvent.button == sf::Mouse::Left) {
        _mouseState.leftPressed = true;
        
        // Guardar posición inicial para el drag
        _dragStartWorldPos = camera.screenToWorld(_mouseState.position, window);
        _cameraStartPos = camera.getPosition();
        
        _currentState = InputState::DRAGGING;
        
    } else if (mouseEvent.button == sf::Mouse::Right) {
        _mouseState.rightPressed = true;
        
        // Click derecho - seleccionar tile
        sf::Vector2f worldPos = camera.screenToWorld(_mouseState.position, window);
        if (_onTileClick) {
            _onTileClick(worldPos);
        }
    }
}

void InputHandler::handleMouseRelease(const sf::Event::MouseButtonEvent& mouseEvent) {
    if (mouseEvent.button == sf::Mouse::Left) {
        _mouseState.leftPressed = false;
        _currentState = InputState::IDLE;
        
    } else if (mouseEvent.button == sf::Mouse::Right) {
        _mouseState.rightPressed = false;
    }
}

void InputHandler::handleMouseMove(const sf::Event::MouseMoveEvent& mouseEvent,
                                  const sf::RenderWindow& window,
                                  const CameraController& camera) {
    _mouseState.lastPosition = _mouseState.position;
    _mouseState.position = sf::Vector2i(mouseEvent.x, mouseEvent.y);
    
    if (_currentState == InputState::DRAGGING && _mouseState.leftPressed) {
        // Calcular el desplazamiento en coordenadas del mundo de forma más eficiente
        sf::Vector2f currentWorldPos = camera.screenToWorld(_mouseState.position, window);
        
        // Usar desplazamiento incremental en lugar de recalcular desde el inicio cada vez
        sf::Vector2f lastWorldPos = camera.screenToWorld(_mouseState.lastPosition, window);
        sf::Vector2f delta = lastWorldPos - currentWorldPos;
        
        if (_onCameraMove) {
            _onCameraMove(camera.getPosition() + delta);
        }
    }
}

void InputHandler::handleMouseWheel(const sf::Event::MouseWheelScrollEvent& wheelEvent) {
    if (wheelEvent.wheel == sf::Mouse::VerticalWheel) {
        float zoomFactor = 1.0f;
        
        if (wheelEvent.delta > 0) {
            // Scroll up - zoom in
            zoomFactor = 0.9f;
        } else if (wheelEvent.delta < 0) {
            // Scroll down - zoom out
            zoomFactor = 1.1f;
        }
        
        if (_onZoom) {
            _onZoom(zoomFactor);
        }
    }
}

void InputHandler::update(float deltaTime) {
    // Podemos añadir interpolación aquí si es necesario
}

void InputHandler::setCameraMoveCallback(std::function<void(const sf::Vector2f&)> callback) {
    _onCameraMove = callback;
}

void InputHandler::setZoomCallback(std::function<void(float)> callback) {
    _onZoom = callback;
}

void InputHandler::setTileClickCallback(std::function<void(const sf::Vector2f&)> callback) {
    _onTileClick = callback;
}