#pragma once 
#include <vector>
#include <glm/glm.hpp>
#include <flecs.h>
#include "collision.hpp"

struct Position
{
    float x;
    float y;
    float angle;
};

struct Player
{
   bool a;
};

struct Walls {
    std::vector<std::pair<glm::vec2, glm::vec2>> walls;
};

using CollisionQuery = flecs::query<Collider, Walls>;
using WallsQuery = flecs::query<Walls>;

struct Attacks
{
    bool a;
};


enum EnemyType
{

};

struct Enemy
{
    bool s;
    //EnemyType type {EnemyType::ATTACKING};
};

