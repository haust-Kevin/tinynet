cmake_minimum_required(VERSION 2.6)

project(tinynet CXX)

enable_testing()

set(CXX_FLAGS
     -std=c++11
)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

include_directories(${PROJECT_SOURCE_DIR})

add_subdirectory(tinynet/base)
add_subdirectory(tinynet/net)
add_subdirectory(tinynet/base/tests)
add_subdirectory(tinynet/net/tests)
