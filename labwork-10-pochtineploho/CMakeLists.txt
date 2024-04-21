cmake_minimum_required(VERSION 3.23)
project(labwork_10_pochtineploho)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXE_LINKER_FLAGS "--static")

if (WIN32)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

include_directories(.)

add_executable(labwork_10_pochtineploho
        BoWeather.cpp
        main.cpp)

include(FetchContent)

FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
        GIT_TAG 742fb6f1a5ed40efe8f3a994e0559c6aff52f2d7)

FetchContent_Declare(nlohmann_json GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG 6af826d0bdb55e4b69e3ad817576745335f243ca)

FetchContent_Declare(ftxui GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI.git
        GIT_TAG 898b8d672ef2cea386048b79276db26867997bae)

FetchContent_MakeAvailable(cpr)
FetchContent_MakeAvailable(nlohmann_json)
FetchContent_MakeAvailable(ftxui)

target_link_libraries(labwork_10_pochtineploho PRIVATE cpr::cpr)
target_link_libraries(labwork_10_pochtineploho PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(labwork_10_pochtineploho PRIVATE ftxui::dom ftxui::component)