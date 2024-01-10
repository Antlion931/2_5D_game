#include <gtest/gtest.h>
#include <SFML/Graphics.hpp>
#include "collision.hpp"

ConvexShapeCollider player_collider() {
    auto collider = sf::ConvexShape{4};
    collider.setPoint(0, sf::Vector2f{0, 0});
    collider.setPoint(1, sf::Vector2f{0, 40});
    collider.setPoint(2, sf::Vector2f{40, 40});
    collider.setPoint(3, sf::Vector2f{40, 0});
    collider.setOrigin(sf::Vector2f{20, 20});

    return ConvexShapeCollider{collider};
}

ConvexShapeCollider wall_collider() {
    auto collider = sf::ConvexShape{4};
    collider.setPoint(0, sf::Vector2f{0, 0});
    collider.setPoint(1, sf::Vector2f{0, 400});
    collider.setPoint(2, sf::Vector2f{40, 400});
    collider.setPoint(3, sf::Vector2f{40, 0});
    collider.setOrigin(sf::Vector2f{20, 200});

    return ConvexShapeCollider{collider};
}

TEST(collision, circle_on_circle_collision)
{
    sf::CircleShape circle1{10};
    sf::CircleShape circle2{10};

    circle1.setPosition(sf::Vector2f{0, 0});
    circle2.setPosition(sf::Vector2f{5, 5});


    auto cc1 = CircleCollider{circle1};
    auto cc2 = CircleCollider{circle2};

    auto result = cc1.collidesWith(&cc2);

    EXPECT_TRUE(get<0>(result));

    auto positions = positionOfABToResolveCollision(&cc1, &cc2);

    cc1.shape.setPosition(get<0>(positions));
    cc2.shape.setPosition(get<1>(positions));

    result = cc1.collidesWith(&cc2);

    EXPECT_FALSE(get<0>(result));
}

TEST(collision, player_on_right_eage_of_wall)
{
    auto player = player_collider();
    auto wall = wall_collider();

    player.shape.setPosition(sf::Vector2f{23.1905, 6.39029});
    player.shape.setRotation(-110.056);
    wall.shape.setPosition(sf::Vector2f{0, 0});

    auto result = player.collidesWith(&wall);

    EXPECT_TRUE(get<0>(result));

    auto position = positionOfAToResolveCollisionWithB(&player, &wall);

    player.shape.setPosition(position);

    result = player.collidesWith(&wall);

    EXPECT_FALSE(get<0>(result));
}

TEST(collision, player_outside_of_wall)
{
    auto player = player_collider();
    auto wall = wall_collider();

    player.shape.setPosition(sf::Vector2f{108.856, 31.3416});
    player.shape.setRotation(-116.613);
    wall.shape.setPosition(sf::Vector2f{0, 0});

    EXPECT_FALSE(get<0>(player.collidesWith(&wall)));
}

TEST(collision, player_on_left_eage_of_wall)
{
    auto player = player_collider();
    auto wall = wall_collider();

    player.shape.setPosition(sf::Vector2f{-31.3933, -50.0738});
    player.shape.setRotation(-113.44);
    wall.shape.setPosition(sf::Vector2f{0, 0});

    auto result = player.collidesWith(&wall);

    EXPECT_TRUE(get<0>(result));

    auto position = positionOfAToResolveCollisionWithB(&player, &wall);

    player.shape.setPosition(position);

    result = player.collidesWith(&wall);

    EXPECT_FALSE(get<0>(result));
}

TEST(collision, player_on_bottom_eage_of_wall)
{
    auto player = player_collider();
    auto wall = wall_collider();

    player.shape.setPosition(sf::Vector2f{-7.6891, 223.008});
    player.shape.setRotation(46.41);
    wall.shape.setPosition(sf::Vector2f{0, 0});

    auto result = player.collidesWith(&wall);

    EXPECT_TRUE(get<0>(result));

    auto position = positionOfAToResolveCollisionWithB(&player, &wall);

    player.shape.setPosition(position);

    result = player.collidesWith(&wall);

    EXPECT_FALSE(get<0>(result));

}

TEST(collision, player_on_top_eage_of_wall)
{
    auto player = player_collider();
    auto wall = wall_collider();

    player.shape.setPosition(sf::Vector2f{3.0382, -220.059});
    player.shape.setRotation(-131.976);
    wall.shape.setPosition(sf::Vector2f{0, 0});

    auto result = player.collidesWith(&wall);

    EXPECT_TRUE(get<0>(result));

    auto position = positionOfAToResolveCollisionWithB(&player, &wall);

    player.shape.setPosition(position);

    result = player.collidesWith(&wall);

    EXPECT_FALSE(get<0>(result));
}
