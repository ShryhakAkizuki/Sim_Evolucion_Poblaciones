# Configuración para tests
include(GoogleTest)

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

# -----------------------------
# Double Linked List - Testing
# -----------------------------

add_executable(test_Double_Linked_List
    data_structures/test_Double_Linked_List.cpp
)

# Incluir directorios
target_include_directories(test_Double_Linked_List
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

# Enlazar con GoogleTest
target_link_libraries(test_Double_Linked_List
    PRIVATE
        GTest::gtest
        GTest::gtest_main
)

# Opciones de compilación para tests
target_compile_options(test_Double_Linked_List
    PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
        $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic -Wno-gnu-zero-variadic-macro-arguments>
        $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
)

# Añadir test al CTest
gtest_discover_tests(test_Double_Linked_List
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# -----------------------------
# Linked Queue - Testing
# -----------------------------

add_executable(test_Linked_Queue
    data_structures/test_Linked_Queue.cpp
)

# Incluir directorios
target_include_directories(test_Linked_Queue
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

# Enlazar con GoogleTest
target_link_libraries(test_Linked_Queue
    PRIVATE
        GTest::gtest
        GTest::gtest_main
)

# Opciones de compilación para tests
target_compile_options(test_Linked_Queue
    PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
        $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic -Wno-gnu-zero-variadic-macro-arguments>
        $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
)

# Añadir test al CTest
gtest_discover_tests(test_Linked_Queue
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# -----------------------------
# Pair - Testing
# -----------------------------

add_executable(test_Pair
    data_structures/test_Pair.cpp
)

# Incluir directorios
target_include_directories(test_Pair
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

# Enlazar con GoogleTest
target_link_libraries(test_Pair
    PRIVATE
        GTest::gtest
        GTest::gtest_main
)

# Opciones de compilación para tests
target_compile_options(test_Pair
    PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
        $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic -Wno-gnu-zero-variadic-macro-arguments>
        $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
)

# Añadir test al CTest
gtest_discover_tests(test_Pair
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# -----------------------------
# Unordered Map - Testing
# -----------------------------

add_executable(test_Unordered_map
    data_structures/test_Unordered_map.cpp
)

# Incluir directorios
target_include_directories(test_Unordered_map
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

# Enlazar con GoogleTest
target_link_libraries(test_Unordered_map
    PRIVATE
        GTest::gtest
        GTest::gtest_main
)

# Opciones de compilación para tests
target_compile_options(test_Unordered_map
    PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
        $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic -Wno-gnu-zero-variadic-macro-arguments>
        $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
)

# Añadir test al CTest
gtest_discover_tests(test_Unordered_map
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)