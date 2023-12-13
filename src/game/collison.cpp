#include "collision.hpp"
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <limits>
#include <optional>
#include <math.h>

float dotProduct(sf::Vector2f a, sf::Vector2f b) {
    return a.x * b.x + a.y * b.y;
}

sf::Vector2f Collider::updatePositionToResolveCollision(Collider* other) {
    auto result = this->collidesWith(other);

    if (get<0>(result)) {
        auto movement = get<0>(get<1>(result).value());

        movement *= get<1>(get<1>(result).value());
        movement *= -1.1f;

        this->shape.move(movement);

        if (get<0>(this->collidesWith(other))) {
            movement *= -1.0f;
            this->shape.move(movement*2.0f);
        }
    }

    return this->shape.getPosition();
}

std::tuple<bool, std::optional<std::tuple<sf::Vector2f, float>>> Collider::collidesWith(Collider* other) {
    auto thisToOther = this->SATCollision(other);
    auto otherToThis = other->SATCollision(this);

    if (!get<0>(thisToOther)) {
        return thisToOther;
    } 

    if (!get<0>(otherToThis)) {
        return otherToThis;
    }

    if (get<1>(get<1>(thisToOther).value()) <= get<1>(get<1>(otherToThis).value())) {
        return thisToOther;
    } else {
        return otherToThis;
    }
}

std::tuple<bool, std::optional<std::tuple<sf::Vector2f, float>>> Collider::SATCollision(Collider* other) {
    auto thisTransform = this->shape.getTransform();

    sf::Vector2f MTV;
    float minOverlap = std::numeric_limits<float>::infinity();

    for (int i = 0; i < this->shape.getPointCount(); i++) {
        sf::Vector2f point1 = thisTransform.transformPoint(this->shape.getPoint(i));
        sf::Vector2f point2 = thisTransform.transformPoint(this->shape.getPoint((i + 1) % this->shape.getPointCount()));
        sf::Vector2f edge = point2 - point1;
        sf::Vector2f normal = sf::Vector2f(edge.y, -edge.x);

        float magnitude = sqrt(normal.x * normal.x + normal.y * normal.y);

        normal /= magnitude;

        float minThis = std::numeric_limits<float>::infinity();
        float maxThis = -std::numeric_limits<float>::infinity();

        for (int j = 0; j < this->shape.getPointCount(); j++) {
            float projection = dotProduct(thisTransform.transformPoint(this->shape.getPoint(j)), normal);
            if (projection < minThis) {
                minThis = projection;
            }
            if (projection > maxThis) {
                maxThis = projection;
            }
        }

        float minOther = std::numeric_limits<float>::infinity();
        float maxOther = -std::numeric_limits<float>::infinity();

        for (int j = 0; j < other->shape.getPointCount(); j++) {
            float projection = dotProduct(other->shape.getTransform().transformPoint(other->shape.getPoint(j)), normal);
            if (projection < minOther) {
                minOther = projection;
            }
            if (projection > maxOther) {
                maxOther = projection;
            }
        }

        float overlap = std::min(maxThis, maxOther) - std::max(minThis, minOther);

        if (overlap <= 0) {
            return std::make_tuple(false, std::nullopt);
        }

        if (overlap < minOverlap) {
            minOverlap = overlap;
            MTV = normal;
        }
    }

    return std::make_tuple(true, std::make_tuple(MTV, minOverlap));
}

