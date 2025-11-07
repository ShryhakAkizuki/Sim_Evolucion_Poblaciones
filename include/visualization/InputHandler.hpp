#pragma once
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <functional>
#include <unordered_map>

#include "visualization/CameraController.hpp"


enum class InputState {
    IDLE,
    DRAGGING,
    ZOOMING
};

struct MouseState {
    sf::Vector2i position;
    sf::Vector2i lastPosition;
    bool leftPressed;
    bool rightPressed;
    bool middlePressed;
};

class InputHandler {
private:
    InputState _currentState;
    MouseState _mouseState;
    
    // Para el drag
    sf::Vector2f _dragStartWorldPos;
    sf::Vector2f _cameraStartPos;
    
    // Callbacks
    std::function<void(const sf::Vector2f&)> _onCameraMove;
    std::function<void(float)> _onZoom;
    std::function<void(const sf::Vector2f&)> _onTileClick;
    
    void handleMousePress(const sf::Event::MouseButtonEvent& mouseEvent, 
                         const sf::RenderWindow& window, 
                         const CameraController& camera);
    void handleMouseRelease(const sf::Event::MouseButtonEvent& mouseEvent);
    void handleMouseMove(const sf::Event::MouseMoveEvent& mouseEvent,
                        const sf::RenderWindow& window,
                        const CameraController& camera);
    void handleMouseWheel(const sf::Event::MouseWheelScrollEvent& wheelEvent);

public:
    InputHandler();
    
    void handleEvent(const sf::Event& event, 
                    const sf::RenderWindow& window,
                    const CameraController& camera);
    void update(float deltaTime);
    
    // Setters para callbacks
    void setCameraMoveCallback(std::function<void(const sf::Vector2f&)> callback);
    void setZoomCallback(std::function<void(float)> callback);
    void setTileClickCallback(std::function<void(const sf::Vector2f&)> callback);
    
    InputState getState() const { return _currentState; }
};