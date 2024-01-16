#include "collision.hpp"
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <limits>
#include <optional>
#include <math.h>
#include <iostream>

float dotProduct(sf::Vector2f a, sf::Vector2f b) {
    return a.x * b.x + a.y * b.y;
}

sf::Vector2f positionOfAToResolveCollisionWithB(ConvexShapeCollider* a, ConvexShapeCollider* b) {
    auto result = a->collidesWith(b);

    auto position = a->shape.getPosition();

    if (get<0>(result)) {
        auto movement = get<0>(get<1>(result).value());

        movement *= get<1>(get<1>(result).value());
        movement *= 1.01f;

        a->shape.move(movement);

        if (get<0>(a->collidesWith(b))) {
            a->shape.move(movement*-2.0f);
        }

        auto buffor = a->shape.getPosition();
        a->shape.setPosition(position);
        position = buffor;
    }

    return position;
}

sf::Vector2f positionOfAToResolveCollisionWithB(CircleCollider* a, ConvexShapeCollider* b) {
    //TODO
    
    return a->shape.getPosition();
}

std::tuple<sf::Vector2f, sf::Vector2f> positionOfABToResolveCollision(CircleCollider* a, CircleCollider* b) {
    auto result = a->collidesWith(b);

    auto a_position = a->shape.getPosition();
    auto b_position = b->shape.getPosition();

    if (get<0>(result)) {
        auto movement = get<0>(get<1>(result).value());

        movement *= get<1>(get<1>(result).value());
        movement *= 1.01f;

        a->shape.move(movement*0.5f);
        b->shape.move(movement*-0.5f);

        if (get<0>(a->collidesWith(b))) {
            a->shape.move(movement*-1.0f);
            b->shape.move(movement*1.0f);
        }

        auto buffor = a->shape.getPosition();
        a->shape.setPosition(a_position);
        a_position = buffor;

        buffor = b->shape.getPosition();
        b->shape.setPosition(b_position);
        b_position = buffor;
    }

    return std::make_tuple(a_position, b_position);
}

std::optional<sf::Vector2f> pointOfIntersection(LineCollider* a, LineCollider* b) {
    //TODO
    
    return std::nullopt;
}

std::tuple<bool, std::optional<std::tuple<sf::Vector2f, float>>> ConvexShapeCollider::collidesWith(ConvexShapeCollider* other) {
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

std::tuple<bool, std::optional<std::tuple<sf::Vector2f, float>>> ConvexShapeCollider::SATCollision(ConvexShapeCollider* other) {
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

std::tuple<bool, std::optional<float>> ConvexShapeCollider::collidesWith(LineCollider* other) {
    //TODO

    return std::make_tuple(false, std::nullopt);
}

std::tuple<bool, std::optional<std::tuple<sf::Vector2f, float>>> ConvexShapeCollider::collidesWith(CircleCollider* other) {
    //TODO

    return std::make_tuple(false, std::nullopt);
}

std::tuple<bool, std::optional<float>> LineCollider::collidesWith(ConvexShapeCollider* other) {
    //TODO

    return std::make_tuple(false, std::nullopt);
}

std::tuple<bool, std::optional<float>> LineCollider::collidesWith(CircleCollider* other) {
    int length_up_to_center = dotProduct(this->end - this->start, other->shape.getPosition() - this->start);

    std::cout << length_up_to_center << std::endl;

    return std::make_tuple(false, std::nullopt);
}

std::tuple<bool, std::optional<std::tuple<sf::Vector2f, float>>> CircleCollider::collidesWith(ConvexShapeCollider* other) {
    //TODO

    return std::make_tuple(false, std::nullopt);
}

std::tuple<bool, std::optional<float>> CircleCollider::collidesWith(LineCollider* other) {
    //TODO

    return std::make_tuple(false, std::nullopt);
}

std::tuple<bool, std::optional<std::tuple<sf::Vector2f, float>>> CircleCollider::collidesWith(CircleCollider* other) {
    this->shape.getPosition();
    other->shape.getPosition();

    float distance = sqrt(pow(this->shape.getPosition().x - other->shape.getPosition().x, 2) + pow(this->shape.getPosition().y - other->shape.getPosition().y, 2));

    if (distance < this->shape.getRadius() + other->shape.getRadius()) {
        sf::Vector2f MTV = this->shape.getPosition() - other->shape.getPosition();
        float magnitude = sqrt(MTV.x * MTV.x + MTV.y * MTV.y);
        MTV /= magnitude;

        return std::make_tuple(true, std::make_tuple(MTV, this->shape.getRadius() + other->shape.getRadius() - distance));
    } else {
        return std::make_tuple(false, std::nullopt);
    }
}

