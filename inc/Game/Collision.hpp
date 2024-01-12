#pragma once 
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics.hpp>
#include <optional>
#include <vector>

struct Collider {
    sf::ConvexShape shape;
    std::tuple<bool, std::optional<std::tuple<sf::Vector2f, float>>> collidesWith(Collider* other);
    std::tuple<bool, std::optional<std::tuple<sf::Vector2f, float>>> SATCollision(Collider* other);
    sf::Vector2f updatePositionToResolveCollision(Collider* other);
};

