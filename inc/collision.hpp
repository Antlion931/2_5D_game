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
std::optional<sf::Vector2f> pointOfIntersection(LineCollider* a, LineCollider* b);

struct ConvexShapeCollider {
    sf::ConvexShape shape;

    std::tuple<bool, std::optional<std::tuple<sf::Vector2f, float>>> collidesWith(ConvexShapeCollider* other);
    //Warning, this will not work when the line doesn't intersect any edge of the shape
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

    //Warning, this will not work when the line doesn't intersect any edge of the shape
    std::tuple<bool, std::optional<std::tuple<sf::Vector2f, float>>> collidesWith(ConvexShapeCollider* other);
    std::tuple<bool, std::optional<float>> collidesWith(LineCollider* other);
    std::tuple<bool, std::optional<std::tuple<sf::Vector2f, float>>> collidesWith(CircleCollider* other);
};
