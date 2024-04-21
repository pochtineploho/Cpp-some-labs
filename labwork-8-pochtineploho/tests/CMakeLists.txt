include(FetchContent)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
        containers_tests
        circular_buffer_ext_test.cpp
        circular_buffer_test.cpp
)

target_link_libraries(
        containers_tests
        circular_buffer
        circular_buffer_ext
        GTest::gtest_main
)

target_include_directories(containers_tests PUBLIC ${PROJECT_SOURCE_DIR})

include(GoogleTest)

gtest_discover_tests(containers_tests)