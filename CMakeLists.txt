cmake_minimum_required(VERSION 3.28)
project(Sim_Evolucion_Poblaciones LANGUAGES CXX)

# --- Configuración general ---
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# --- Limpieza automática de build antes de compilar ---
add_custom_target(clean_all
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/*
    COMMENT "Limpiando compilación anterior..."
)
add_custom_target(rebuild
    COMMAND ${CMAKE_COMMAND} --build . --target clean_all
    COMMAND ${CMAKE_COMMAND} --build .
    COMMENT "Reconstruyendo proyecto completo..."
)

# --- Cargar dependencias externas ---
include(FetchContent)

# ✅ Traer SFML 2.6.1 directamente desde GitHub
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.1
)
FetchContent_MakeAvailable(SFML)

# ✅ OpenGL (para futuras integraciones visuales)
find_package(OpenGL REQUIRED)

# --- Archivos fuente ---
file(GLOB_RECURSE SOURCES "src/*.cpp" "include/*.hpp")

# --- Ejecutable principal ---
add_executable(${PROJECT_NAME} ${SOURCES})

# --- Vinculación de librerías ---
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        sfml-system
        sfml-window
        sfml-graphics
        sfml-audio
        sfml-network
        OpenGL::GL
)

# --- Include path ---
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include)

# --- Mensajes de estado ---
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Output path: ${EXECUTABLE_OUTPUT_PATH}")
message(STATUS "SFML fetched from GitHub tag 2.6.1")

# --- Copiar DLLs en Windows ---
if (WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${sfml_BINARY_DIR}/lib/$<CONFIG>"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/dlls"
    )
endif()