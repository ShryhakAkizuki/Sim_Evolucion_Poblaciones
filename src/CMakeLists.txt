# -----------------------------
# Engine
# -----------------------------
add_library(engine
    biome/Biome.cpp
    biome/BiomeSystem.cpp
    map/WorldGenerator.cpp
    # visualization/CameraController.cpp
    # visualization/GameWindows.cpp
    # visualization/InputHandler.cpp
    # visualization/MapRenderer.cpp
    # visualization/TileDrawer.cpp
    # visualization/ViewManager.cpp
)

target_include_directories(engine
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(engine
    PUBLIC
        glad
        glfw
        imgui
        glm::glm
        stb
)

# -----------------------------
# App
# -----------------------------
add_executable(app main.cpp)

target_link_libraries(app PRIVATE engine)

target_compile_options(engine PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>
)

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()
