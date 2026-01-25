# -----------------------------
# Biome_Engine
# -----------------------------
add_library(biome_engine
    biome/Biome.cpp
    biome/BiomeSystem.cpp
)

target_include_directories(biome_engine PUBLIC ${PROJECT_SOURCE_DIR}/include)
# -----------------------------
# Map_Engine
# -----------------------------
add_library(map_engine
    map/generator/WorldGenerator.cpp
    map/manager/ChunkManager.cpp
    map/WorldSystem.cpp
)

target_include_directories(map_engine PUBLIC ${PROJECT_SOURCE_DIR}/include)
# -----------------------------
# Graphics_Engine
# -----------------------------
add_library(graphics_engine
    graphics/Camera.cpp
    graphics/InputManager.cpp
    graphics/TileRenderer.cpp
    graphics/RenderSystem.cpp
)

target_include_directories(graphics_engine PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(graphics_engine
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

target_link_libraries(app 
    PRIVATE 
        biome_engine
        map_engine
        graphics_engine
)

foreach(tgt biome_engine map_engine graphics_engine)
    target_compile_options(${tgt} PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
        $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>
    )
endforeach()

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()
