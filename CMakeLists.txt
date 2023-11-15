cmake_minimum_required(VERSION 3.27)

project(2_5D_GAME)

# C++ setup

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# GTest setup

enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# SFML setup

find_package(SFML 2.5 COMPONENTS graphics audio system window REQUIRED)

# Files setup

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE RESOURCES "resources/*")
file(GLOB_RECURSE TESTS "tst/*.cpp")

# Executable setup

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-audio sfml-system sfml-window)
target_include_directories(${PROJECT_NAME} PUBLIC inc)

# Tests setup

add_executable(${PROJECT_NAME}_ut ${TESTS})
target_link_libraries(${PROJECT_NAME}_ut ${GTEST_BOTH_LIBRARIES})
target_include_directories(${PROJECT_NAME}_ut PUBLIC inc)