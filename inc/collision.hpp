#pragma once 
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics.hpp>
#include <optional>

struct ConvexShapeCollider;
struct LineCollider;
struct CircleCollider;

struct ConvexShapeCollider {
    sf::ConvexShape shape;

    std::tuple<bool, std::optional<std::tuple<sf::Vector2f, float>>> collidesWith(ConvexShapeCollider* other);
    std::tuple<bool, std::optional<std::tuple<sf::Vector2f, float>>> collidesWith(LineCollider* other);
    std::tuple<bool, std::optional<std::tuple<sf::Vector2f, float>>> collidesWith(CircleCollider* other);

    std::tuple<bool, std::optional<std::tuple<sf::Vector2f, float>>> SATCollision(ConvexShapeCollider* other);

    sf::Vector2f updatePositionToResolveCollision(ConvexShapeCollider* other);
};

struct LineCollider {
    sf::Vector2f start;
    sf::Vector2f end;

    std::tuple<bool, std::optional<std::tuple<sf::Vector2f, float>>> collidesWith(ConvexShapeCollider* other);
    std::tuple<bool, std::optional<std::tuple<sf::Vector2f, float>>> collidesWith(CircleCollider* other);
};

struct CircleCollider {
    sf::CircleShape shape;

    std::tuple<bool, std::optional<std::tuple<sf::Vector2f, float>>> collidesWith(ConvexShapeCollider* other);
    std::tuple<bool, std::optional<std::tuple<sf::Vector2f, float>>> collidesWith(LineCollider* other);
    std::tuple<bool, std::optional<std::tuple<sf::Vector2f, float>>> collidesWith(CircleCollider* other);
};
