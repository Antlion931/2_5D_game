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

# GLAD setup

set(VENDOR_DIR ${CMAKE_SOURCE_DIR}/vendor)

set(GLAD_DIR ${VENDOR_DIR}/GLAD)
set(GLAD_URL "https://gitfront.io/r/Moztanku/8s4L1GHF3Tsc/Resources/raw/glad.zip")

if (NOT EXISTS ${GLAD_DIR})
    file(DOWNLOAD ${GLAD_URL} ${GLAD_DIR}/glad.zip SHOW_PROGRESS
        STATUS status)

    list(GET status 0 error_code)

    if (NOT ${error_code} EQUAL 0)
        file(REMOVE_RECURSE ${GLAD_DIR})
        message(FATAL_ERROR "Failed to download GLAD, generate it through: 'https://gen.glad.sh/#generator=c&api=gl%3D4.6&profile=gl%3Dcore%2Cgles1%3Dcommon&options=LOADER' and place it in ${GLAD_DIR}")
    endif()

    execute_process(
        COMMAND ${CMAKE_COMMAND} -E tar xzf glad.zip
        WORKING_DIRECTORY ${GLAD_DIR}
    )

    file(REMOVE ${GLAD_DIR}/glad.zip)
endif()

add_library(glad STATIC ${GLAD_DIR}/src/gl.c)
target_include_directories(glad PUBLIC ${GLAD_DIR}/include)

# OpenGL setup

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

# GLM setup

find_package(glm REQUIRED)

# Files setup

set(MAIN_GAME "src/Renderer.cpp")
set(LEVEL_ED "src/LevelEditor.cpp")

file(GLOB_RECURSE GAME_SOURCES "src/Game/**.cpp" )
file(GLOB_RECURSE RENDERER_SOURCES "src/Renderer/**.cpp")

set(HEADERS "inc")
set(RESOURCES_HEADERS ${CMAKE_CURRENT_BINARY_DIR}/inc)

file(GLOB_RECURSE RESOURCES "resources/*")
file(GLOB_RECURSE TESTS "tst/*.cpp")

# Add flecs

add_subdirectory (flecs)

# Create resource files

add_subdirectory(ResourceLoader)

# Game setup

add_executable(${PROJECT_NAME} ${MAIN_GAME})

target_include_directories(
    ${PROJECT_NAME} PRIVATE
        ${HEADERS}
        ${RESOURCES_HEADERS}
    )

target_sources(
    ${PROJECT_NAME} PRIVATE
        # ${GAME_SOURCES}
        ${RENDERER_SOURCES}
    )

target_link_libraries(
    ${PROJECT_NAME} PRIVATE
        glfw
        GL
        glad 
        resources 
        flecs_static
    )

# Level editor

add_executable(${PROJECT_NAME}_LE ${LEVEL_ED})

target_include_directories(
    ${PROJECT_NAME}_LE PRIVATE
        ${HEADERS}
        ${RESOURCES_HEADERS}
    )

target_sources(
    ${PROJECT_NAME}_LE PRIVATE
    )

target_link_libraries(
    ${PROJECT_NAME}_LE PRIVATE
    sfml-graphics
    sfml-audio
    sfml-system
    sfml-window
    )

# Tests setup

# add_executable(${PROJECT_NAME}_ut ${TESTS} ${GAME_SOURCES} ${RENDERER_SOURCES})
# target_link_libraries(${PROJECT_NAME}_ut ${GTEST_BOTH_LIBRARIES})
# target_include_directories(${PROJECT_NAME}_ut PUBLIC inc ${CMAKE_CURRENT_BINARY_DIR}/inc)
# target_link_libraries(${PROJECT_NAME}_ut flecs sfml-graphics sfml-audio sfml-system sfml-window GL resources)
# target_link_libraries(${PROJECT_NAME}_ut flecs_static)

# Makedir for levels

file(MAKE_DIRECTORY levels)