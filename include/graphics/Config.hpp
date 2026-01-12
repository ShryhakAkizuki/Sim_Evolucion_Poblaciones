#pragma once

struct GraphicsConfig {
    // Ventana
    int windowWidth = 1920;
    int windowHeight = 1080;
    const char* windowTitle = "World Generator";
    bool vsync = false;
    bool fullscreen = false;
    bool resizable = true;
    
    // Renderizado
    int tileSize = 32;
    int maxChunksPerFrame = 64;
    
    // Cámara
    float cameraMoveSpeed = 1000.0f;
    float cameraZoomSpeed = 0.1f;
    float minZoom = 0.1f;
    float maxZoom = 10.0f;
    
    // Visuales
    bool showChunkBorders = true;
    bool showActivityCenters = true;
    bool showBiomeColors = true;
    bool showWater = true;
    bool wireframeMode = false;
    bool showGrid = false;
    
    // Colores
    struct Colors {
        float backgroundColor[4] = {0.1f, 0.1f, 0.12f, 1.0f};
        float waterColor[4] = {0.25f, 0.64f, 0.87f, 0.7f};
        float chunkBorderColor[4] = {0.2f, 0.2f, 0.2f, 0.4f};
        float gridColor[4] = {0.3f, 0.3f, 0.3f, 0.2f};
        float activityCenterColor[4] = {1.0f, 0.0f, 0.0f, 0.6f};
    } colors;
};
