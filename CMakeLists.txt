cmake_minimum_required(VERSION 3.27)

project(2_5D_GAME)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# C++ setup

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# GTest setup

enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# SFML setup

find_package(SFML COMPONENTS graphics audio system window REQUIRED)

# OpenGL setup

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

# GLM setup

find_package(glm REQUIRED)

# Files setup

file(GLOB_RECURSE MAIN_GAME "src/main.cpp" )
file(GLOB_RECURSE GAME_SOURCES "src/game/**.cpp" )
file(GLOB_RECURSE RENDERER_SOURCES "src/Renderer/**.cpp" )
file(GLOB_RECURSE RESOURCES "resources/*")
file(GLOB_RECURSE TESTS "tst/*.cpp")

# Add flecs

add_subdirectory (flecs)

# Create resource files

add_subdirectory(ResourceLoader)

# Executable setup

add_executable(${PROJECT_NAME} ${GAME_SOURCES} ${RENDERER_SOURCES} ${MAIN_GAME})
target_include_directories(${PROJECT_NAME} PUBLIC inc ${CMAKE_CURRENT_BINARY_DIR}/inc)
target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-audio sfml-system sfml-window GL GLEW resources)
target_link_libraries(${PROJECT_NAME} flecs_static)

# Tests setup

add_executable(${PROJECT_NAME}_ut ${TESTS} ${GAME_SOURCES} ${RENDERER_SOURCES})
target_link_libraries(${PROJECT_NAME}_ut ${GTEST_BOTH_LIBRARIES})
target_include_directories(${PROJECT_NAME}_ut PUBLIC inc)
target_link_libraries(${PROJECT_NAME}_ut flecs sfml-graphics sfml-audio sfml-system sfml-window GL GLEW resources)
target_link_libraries(${PROJECT_NAME}_ut flecs_static)
