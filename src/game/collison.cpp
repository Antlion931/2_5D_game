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

    if (result.has_value()) {
        auto movement = get<0>(result.value());

        movement *= get<1>(result.value());
        movement *= 1.01f;

        a->shape.move(movement);

        if (a->collidesWith(b).has_value()) {
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

    if (result.has_value()) {
        auto movement = get<0>(result.value());

        movement *= get<1>(result.value());
        movement *= 1.01f;

        a->shape.move(movement*0.5f);
        b->shape.move(movement*-0.5f);

        if (a->collidesWith(b).has_value()) {
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
    auto x1 = a->start.x;
    auto y1 = a->start.y;
    auto x2 = a->end.x;
    auto y2 = a->end.y;

    auto x3 = b->start.x;
    auto y3 = b->start.y;
    auto x4 = b->end.x;
    auto y4 = b->end.y;
    
    float uA = ((x4-x3)*(y1-y3) - (y4-y3)*(x1-x3)) / ((y4-y3)*(x2-x1) - (x4-x3)*(y2-y1));
    float uB = ((x2-x1)*(y1-y3) - (y2-y1)*(x1-x3)) / ((y4-y3)*(x2-x1) - (x4-x3)*(y2-y1));


    if (uA >= 0 && uA <= 1 && uB >= 0 && uB <= 1) {
        float intersectionX = x1 + (uA * (x2-x1));
        float intersectionY = y1 + (uA * (y2-y1));

        return std::make_optional(sf::Vector2f(intersectionX, intersectionY));
    }

    return std::nullopt;
}

std::optional<std::tuple<sf::Vector2f, float>> ConvexShapeCollider::collidesWith(ConvexShapeCollider* other) {
    auto thisToOther = this->SATCollision(other);
    auto otherToThis = other->SATCollision(this);

    if (!thisToOther.has_value()) {
        return thisToOther;
    } 

    if (!otherToThis.has_value()) {
        return otherToThis;
    }

    if (get<1>(thisToOther.value()) <= get<1>(otherToThis.value())) {
        return thisToOther;
    } else {
        return otherToThis;
    }
}

std::optional<std::tuple<sf::Vector2f, float>> ConvexShapeCollider::SATCollision(ConvexShapeCollider* other) {
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
            return std::nullopt;
        }

        if (overlap < minOverlap) {
            minOverlap = overlap;
            MTV = normal;
        }
    }

    return std::make_tuple(MTV, minOverlap);
}

std::optional<float> ConvexShapeCollider::collidesWith(LineCollider* other) {
    std::optional<float> minDistance = std::nullopt;

    for (int i = 0; i < this->shape.getPointCount(); i++) {
        sf::Vector2f point1 = this->shape.getTransform().transformPoint(this->shape.getPoint(i));
        sf::Vector2f point2 = this->shape.getTransform().transformPoint(this->shape.getPoint((i + 1) % this->shape.getPointCount()));

        LineCollider lineCollider(point1, point2);

        auto result = pointOfIntersection(&lineCollider, other);

        if (result.has_value()) {
            auto distance = sqrt(pow(other->start.x - result.value().x, 2) + pow(other->start.y - result.value().y, 2));

            if (!minDistance.has_value() || distance < minDistance.value()) {
                minDistance = std::make_optional(distance);
            }
        }
    }

    return minDistance;
}

std::optional<std::tuple<sf::Vector2f, float>> ConvexShapeCollider::collidesWith(CircleCollider* other) {
    //TODO

    return std::nullopt;
}

std::optional<float> LineCollider::collidesWith(ConvexShapeCollider* other) {
    return other->collidesWith(this);
}

std::optional<float> LineCollider::collidesWith(CircleCollider* other) {
    auto normal = this->end - this->start;
    normal /= sqrt(normal.x * normal.x + normal.y * normal.y);
    auto length_up_to_center = dotProduct(normal, other->shape.getPosition() - this->start);

    auto length = sqrt(pow(this->end.x - this->start.x, 2) + pow(this->end.y - this->start.y, 2));

    if (length_up_to_center < -other->shape.getRadius() || length_up_to_center > length + other->shape.getRadius()) {
        return std::nullopt;
    }

    auto rotated_normal = sf::Vector2f(normal.y, -normal.x);

    auto line_on_rn = dotProduct(rotated_normal, this->start);
    auto circle_on_rn = dotProduct(rotated_normal, other->shape.getPosition());

    if (abs(line_on_rn - circle_on_rn) > other->shape.getRadius()) {
        return std::nullopt;
    }

    auto how_deep_line_is_in_circle = sqrt(pow(other->shape.getRadius(), 2) - pow(line_on_rn - circle_on_rn, 2));

    auto new_length = fmin(fmax(length_up_to_center - how_deep_line_is_in_circle, 0.0f), length);

    return std::make_optional(new_length);
}

std::optional<std::tuple<sf::Vector2f, float>> CircleCollider::collidesWith(ConvexShapeCollider* other) {
    //TODO

    return std::nullopt;
}

std::optional<float> CircleCollider::collidesWith(LineCollider* other) {
    return other->collidesWith(this);
}

std::optional<std::tuple<sf::Vector2f, float>> CircleCollider::collidesWith(CircleCollider* other) {
    this->shape.getPosition();
    other->shape.getPosition();

    float distance = sqrt(pow(this->shape.getPosition().x - other->shape.getPosition().x, 2) + pow(this->shape.getPosition().y - other->shape.getPosition().y, 2));

    if (distance < this->shape.getRadius() + other->shape.getRadius()) {
        sf::Vector2f MTV = this->shape.getPosition() - other->shape.getPosition();
        float magnitude = sqrt(MTV.x * MTV.x + MTV.y * MTV.y);
        MTV /= magnitude;

        return std::make_tuple(MTV, this->shape.getRadius() + other->shape.getRadius() - distance);
    } else {
        return std::nullopt;
    }
}

