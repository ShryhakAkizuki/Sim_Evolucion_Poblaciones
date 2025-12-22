cmake_minimum_required(VERSION 3.24)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

project(Sim_Evolucion_Poblaciones 
        VERSION 0.1 
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --------------------------------------------------
# FetchContent
# --------------------------------------------------
include(FetchContent)

# --------------------------------------------------
# GLFW
# --------------------------------------------------
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glfw)

if (TARGET glfw)
    target_compile_options(glfw PRIVATE
        $<$<C_COMPILER_ID:Clang>:
            -Wno-sign-compare
            -Wno-unused-parameter
            -Wno-missing-field-initializers
        >
        $<$<C_COMPILER_ID:GNU>:
            -Wno-sign-compare
            -Wno-unused-parameter
            -Wno-missing-field-initializers
        >
    )
endif()
# --------------------------------------------------
# GLM (header-only)
# --------------------------------------------------
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.2
)

FetchContent_MakeAvailable(glm)

# --------------------------------------------------
# stb (header-only)
# --------------------------------------------------
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG master
)

FetchContent_MakeAvailable(stb)

add_library(stb INTERFACE)
target_include_directories(stb INTERFACE ${stb_SOURCE_DIR})


# --------------------------------------------------
# Dear ImGui
# --------------------------------------------------
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.92.5
)

FetchContent_MakeAvailable(imgui)

add_library(imgui
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui
    PUBLIC
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(imgui
    PUBLIC
        glfw
)

target_compile_definitions(imgui
    PUBLIC
        IMGUI_IMPL_OPENGL_LOADER_GLAD
)

# --------------------------------------------------
# GLAD
# --------------------------------------------------
FetchContent_Declare( 
    glad 
    GIT_REPOSITORY 
    https://github.com/Dav1dde/glad.git 
    GIT_TAG v0.1.36 ) 
FetchContent_MakeAvailable(glad) 

# --------------------------------------------------
# Subproyecto
# --------------------------------------------------
add_subdirectory(src)

