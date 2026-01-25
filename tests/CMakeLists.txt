# Configuración para tests
find_package(GTest REQUIRED)

# -----------------------------
# Dinamic Array - Testing
# -----------------------------

add_executable(tests_DynamicArray
    data_structures/test_DynamicArray.cpp
)

# Incluir directorios
target_include_directories(tests_DynamicArray
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

# Enlazar con GoogleTest
target_link_libraries(tests_DynamicArray
    PRIVATE
        GTest::gtest
        GTest::gtest_main
)

# Opciones de compilación para tests
target_compile_options(tests_DynamicArray
    PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
        $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic -Wno-gnu-zero-variadic-macro-arguments>
        $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
)

# Añadir test al CTest
gtest_discover_tests(tests_DynamicArray
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# -----------------------------
# Node_Templates - Testing
# -----------------------------

add_executable(test_Node_Templates
    data_structures/test_Node_Templates.cpp
)

# Incluir directorios
target_include_directories(test_Node_Templates
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

# Enlazar con GoogleTest
target_link_libraries(test_Node_Templates
    PRIVATE
        GTest::gtest
        GTest::gtest_main
)

# Opciones de compilación para tests
target_compile_options(test_Node_Templates
    PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
        $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic -Wno-gnu-zero-variadic-macro-arguments>
        $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
)

# Añadir test al CTest
gtest_discover_tests(test_Node_Templates
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)