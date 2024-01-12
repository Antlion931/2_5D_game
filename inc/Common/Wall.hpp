#pragma once

#include <SFML/System/Vector2.hpp>

enum class TextureMode : unsigned int
{
    Stretch,
    Repeat,
    RepeatMirrored
};

static TextureMode& operator++(TextureMode& mode) {
    switch (mode) {
        case TextureMode::Stretch:
            mode = TextureMode::Repeat;
            break;
        case TextureMode::Repeat:
            mode = TextureMode::RepeatMirrored;
            break;
        case TextureMode::RepeatMirrored:
            mode = TextureMode::Stretch;
            break;
        // Add more cases if needed
    }
    return mode;
}

static TextureMode& operator--(TextureMode& mode) {
    switch (mode) {
        case TextureMode::Stretch:
            mode = TextureMode::RepeatMirrored;
            break;
        case TextureMode::Repeat:
            mode = TextureMode::Stretch;
            break;
        case TextureMode::RepeatMirrored:
            mode = TextureMode::Repeat;
            break;
        // Add more cases if needed
    }
    return mode;
}

static std::string textureModeToString(TextureMode mode)
{
    switch (mode)
    {
        case TextureMode::Stretch:
            return "Stretch";
        case TextureMode::Repeat:
            return "Repeat";
        case TextureMode::RepeatMirrored:
            return "RepeatMirrored";
        default:
            return "Unknown";
    }
}

struct Wall
{
    sf::Vector2f a;
    sf::Vector2f b;
    float height;
    unsigned int textureId;
    TextureMode textureMode;
};