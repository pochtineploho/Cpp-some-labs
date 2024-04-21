cmake_minimum_required(VERSION 3.23)

project(labwork_8_pochtineploho)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(lib)
add_subdirectory(bin)

enable_testing()
add_subdirectory(tests)
