#pragma once

#include <optional>

#include <SFML/System/Vector2.hpp>

#include "Common/Wall.hpp"

enum class EditorMode {
    Wall,
    Chunk
};

struct State 
{
    // View
    bool drawGrid = true;
    const float gridWidth = 1.0f;
    const float gridCellSize = 4.0f;
    float zoom = 8.0f;
    sf::Vector2f viewCenter = sf::Vector2f(0.0f, 0.0f);
    // Selection
    bool isDragging = false;
    bool snapToGrid = true;
    sf::Vector2f focusedV = sf::Vector2f(0.0f, 0.0f);
    std::optional<sf::Vector2f> selectedV = std::nullopt;
    // Mode
    EditorMode editorMode = EditorMode::Wall;
    //
    bool isRunning = true;
    bool create = false;
    bool undo = false;
    bool shift = false;
    bool ctrl = false;
    bool chunk_update = false;
    bool save = false;
    bool load = false;
    std::string filename = "test.lvl";
    // Wall
    float height = 1.0f;
    unsigned int textureId = 0;
    TextureMode textureMode = TextureMode::Repeat;
    // Chunk
    float floorHeight = 0.0f;
    bool isCeilingVisible = false;
    float ceilingHeight = 0.0f;
    size_t selectedChunk = 0;
    //
};