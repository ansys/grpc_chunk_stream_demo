project(grpc_benchmark)
cmake_minimum_required(VERSION 3.1)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

set(CMAKE_CXX_FLAGS "-O3 -std=c++20 -Wall -Wextra -Wpedantic -Werror")
# set(SRC, ${PROJECT_SOURCE_DIR}/src)

add_subdirectory(generated_src)
add_subdirectory(src)
