cmake_minimum_required(VERSION 4.2)
project(javinha VERSION 1.0.0 LANGUAGES CXX)

# major config
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(APP_NAME "javinha")
set(INCLUDE_DIR "include")

# set compiler flags for both scenarios
set(DEBUG_COMPILE_OPTIONS "-Wall" "-pedantic" "-g3" "-O0" "-std=c++20" "-DDEBUG")
set(RELEASE_COMPILE_OPTIONS "-O3" "-DNDEBUG" "-std=c++20" "-UDEBUG")

# set c++20 required
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

include(FetchContent)
FetchContent_Declare(
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
)
FetchContent_MakeAvailable(argparse)

# sources
file(GLOB SOURCES "src/*.cpp")

# release executable
add_executable(${APP_NAME} ${SOURCES})
target_include_directories(${APP_NAME} PRIVATE ${INCLUDE_DIR})
target_link_libraries(${APP_NAME} argparse)
target_compile_options(${APP_NAME} PRIVATE 
    -Wall
    -Wextra
    -Wpedantic
    $<$<CONFIG:Debug>:
        -g3
        -O0
        -DDEBUG
    >
    $<$<CONFIG:Release>:
        -O3
        -DNDEBUG
    >
)
