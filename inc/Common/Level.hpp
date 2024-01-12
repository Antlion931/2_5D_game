#pragma once

#include <vector>
#include <string>
#include <fstream>

#include "Common/Wall.hpp"
#include "Common/Log.hpp"

struct Level
{
    std::vector<Wall> static_walls;
};

static Level loadLevel(const std::string& fileName)
{
    constexpr auto levels_path = std::string_view("levels/");

    std::ifstream file(levels_path.data() + fileName);

    if (!file.is_open())
    {
        log("Failed to open file " + fileName);

        return Level{};
    }

    Level level;

    size_t wall_count = 0;
    file >> wall_count;

    level.static_walls.reserve(wall_count);

    for (size_t i = 0; i < wall_count; ++i)
    {
        Wall wall;

        file >> wall.a.x >> wall.a.y >> wall.b.x >> wall.b.y >> wall.height >> wall.textureId;

        unsigned int textureMode;
        file >> textureMode;

        wall.textureMode = static_cast<TextureMode>(textureMode);

        level.static_walls.push_back(wall);
    }

    file.close();

    return level;
}

static void saveLevel(const Level& level, const std::string& fileName)
{
    constexpr auto levels_path = std::string_view("levels/");

    std::ofstream file(levels_path.data() + fileName);

    if (!file.is_open())
    {
        log("Failed to open file " + fileName);

        return;
    }

    file << level.static_walls.size() << '\n';

    for (const auto& wall : level.static_walls)
    {
        file << wall.a.x << ' ' << wall.a.y << ' ' << wall.b.x << ' ' << wall.b.y << ' ' << wall.height << ' ' << wall.textureId << ' ' << static_cast<unsigned int>(wall.textureMode) << '\n';
    }

    file.close();
}