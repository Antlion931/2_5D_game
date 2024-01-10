#pragma once 
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics.hpp>
#include <optional>

struct ConvexShapeCollider;
struct LineCollider;
struct CircleCollider;

sf::Vector2f positionOfAToResolveCollisionWithB(ConvexShapeCollider* a, ConvexShapeCollider* b);
sf::Vector2f positionOfAToResolveCollisionWithB(CircleCollider* a, ConvexShapeCollider* b);
std::tuple<sf::Vector2f, sf::Vector2f> positionOfABToResolveCollision(CircleCollider* a, CircleCollider* b);

struct ConvexShapeCollider {
    sf::ConvexShape shape;

    std::tuple<bool, std::optional<std::tuple<sf::Vector2f, float>>> collidesWith(ConvexShapeCollider* other);
    std::tuple<bool, std::optional<float>> collidesWith(LineCollider* other);
    std::tuple<bool, std::optional<std::tuple<sf::Vector2f, float>>> collidesWith(CircleCollider* other);

    std::tuple<bool, std::optional<std::tuple<sf::Vector2f, float>>> SATCollision(ConvexShapeCollider* other);
};

struct LineCollider {
    sf::Vector2f start;
    sf::Vector2f end;

    std::tuple<bool, std::optional<float>> collidesWith(ConvexShapeCollider* other);
    std::tuple<bool, std::optional<float>> collidesWith(CircleCollider* other);

    void updateLengthToResolveCollision(ConvexShapeCollider* other);
};

struct CircleCollider {
    sf::CircleShape shape;

    std::tuple<bool, std::optional<std::tuple<sf::Vector2f, float>>> collidesWith(ConvexShapeCollider* other);
    std::tuple<bool, std::optional<float>> collidesWith(LineCollider* other);
    std::tuple<bool, std::optional<std::tuple<sf::Vector2f, float>>> collidesWith(CircleCollider* other);
};
