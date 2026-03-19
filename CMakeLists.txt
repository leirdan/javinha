cmake_minimum_required(VERSION 4.2)
project(javinha VERSION 1.0.0 LANGUAGES CXX)

# major config
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(APP_NAME "javinha")
set(INCLUDE_DIR "include")

# set compiler flags for both scenarios
set(DEBUG_COMPILE_OPTIONS "-Wall" "-pedantic" "-g3" "-O0")
set(RELEASE_COMPILE_OPTIONS "-O3" "-DNDEBUG")

# set c++17 required
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

# sources
file(GLOB SOURCES "src/*.cpp")
include_directories(${INCLUDE_DIR})

# debug 
add_executable(${APP_NAME}_debug ${SOURCES})
target_compile_options(${APP_NAME}_debug PRIVATE ${DEBUG_COMPILE_OPTIONS})

# release executable
add_executable(${APP_NAME}_release ${SOURCES})
target_compile_options(${APP_NAME}_release PRIVATE ${RELEASE_COMPILE_OPTIONS})
