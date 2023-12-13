#include "game.h"
#include <flecs.h>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <math.h>

struct Position
{
    float x;
    float y;
    float angle;
};


class SFMLWindow
{
public:
    static sf::RenderWindow window;
    sf::View playerView;
    sf::Event event;
    SFMLWindow() : playerView{{0.0, 0.0}, {800.f, 600.f}}
    {
        std::cout << "Singleton instance created." << std::endl;
    }
};

sf::RenderWindow SFMLWindow::window(sf::VideoMode{800, 600}, "2_5D_Game!");


class Ball : public sf::Drawable
{
public:
    Ball(float x, float y)
    {
        shape.setPosition(x, y);
        shape.setRadius(this->radius);
        shape.setFillColor(sf::Color::White);
        shape.setOrigin(sf::Vector2f{this->radius, this->radius});
    }
    Ball() = delete;
    ~Ball() = default;
private:
    sf::CircleShape shape;
    const float radius {10.0f};
    void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        target.draw(this->shape, states);
    }
};


class Rectange : public sf::Drawable
{
public:
    Rectange(float x, float y, float angle)
    {
        shape.setPosition(x, y);
        shape.setSize(sf::Vector2{30.0f, 4.0f});
        shape.setOrigin(sf::Vector2{0.0f, 2.0f});
        shape.setRotation(angle);
        shape.setFillColor(sf::Color::Yellow);
    }

private:
    sf::RectangleShape shape;
    void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        target.draw(this->shape, states);
    }
};



struct Player
{
   bool a;
};



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


struct Health
{

    float level = 100.f;
    void decrease(float value)
    {
        level -= value;
    }

    void increase(float value)
    {
        level += value;
    }
};


void attack(flecs::entity entity, float )
{

}



void run() 
{
    flecs::world world {};
    world.set<SFMLWindow>(SFMLWindow{});

    flecs::entity enemy0 {world.entity()};
    flecs::entity enemy1 {world.entity()};
    flecs::entity enemy3 {world.entity()};

    flecs::entity player {world.entity("Player")};

    player.set<Position>({100, 100, 0}).set<Player>({}).set<Health>({100.f});


    auto playerMoveSys = world.system<Player, Position>()
        .iter([](flecs::iter it, Player* player, Position* position)
        {
            float speed {100};

            for (int i : it)
            {
                bool isMoving {false};
                float deltaAngle {};

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && sf::Keyboard::isKeyPressed(sf::Keyboard::S))
                {
                    continue;
                }

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && sf::Keyboard::isKeyPressed(sf::Keyboard::D))
                {
                    continue;
                }
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::W))
                {
                    isMoving = true;
                    deltaAngle = 0;
                }
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::S))
                {
                    isMoving = true;
                    deltaAngle = 180.0f;
                }
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::A))
                {
                    isMoving = true;
                    deltaAngle = 270.0f;
                }
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::D))
                {
                    isMoving = true;
                    deltaAngle = 90.0f;
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && sf::Keyboard::isKeyPressed(sf::Keyboard::D))
                {
                    isMoving = true;
                    deltaAngle = 45.f;
                }

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && sf::Keyboard::isKeyPressed(sf::Keyboard::A))
                {
                    isMoving = true;
                    deltaAngle = 315.f;
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && sf::Keyboard::isKeyPressed(sf::Keyboard::A))
                {
                    isMoving = true;
                    deltaAngle = 215.f;
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && sf::Keyboard::isKeyPressed(sf::Keyboard::D))
                {
                    isMoving = true;
                    deltaAngle = 135.f;
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
                {
                    position[i].angle += 50 * it.delta_time();
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
                {
                    position[i].angle -= 50 * it.delta_time();
                }
                
                if (isMoving)
                {
                    position[i].x += speed * cosf((position[i].angle + deltaAngle) * M_PI / 180) * it.delta_time(); 
                    position[i].y += speed * sinf((position[i].angle + deltaAngle) * M_PI / 180) * it.delta_time();        
                }
                
                it.world().get_mut<SFMLWindow>()->playerView.setCenter(position[i].x, position[i].y);
            }
            
        });


    enemy0.set(Position{200, 400, 10}).set<Enemy>({}).set<Health>({}).add<Attacks>(player);
    enemy1.set(Position{300, 500, 10}).set<Enemy>({}).set<Health>({}).add<Attacks>(player);
    enemy3.set(Position{520,440, 160}).set<Enemy>({}).set<Health>({}).add<Attacks>(player);



    auto chasePlayerSys = world.system<Enemy, Position>()
    .kind(flecs::OnUpdate)
    .with<Attacks, Player>()
    .each([](flecs::iter& it, size_t i, Enemy& e, Position& enemyPosition)
    {
        flecs::entity player {it.entity(i).target<Attacks>()};
        const Position* playerPosition {player.get<Position>()};
        double distance {sqrt(pow(playerPosition->x - enemyPosition.x, 2) + pow(playerPosition->y - enemyPosition.y, 2))};
        
        float speed {30.f};
        float dirX {};
        float dirY {};
        if (distance < 300.f && distance > 50.f)
        {
            std::cout << "Enemy on [" << enemyPosition.x << ", " <<  enemyPosition.y << " ] Attacks: " << it.entity(i).target<Attacks>().name() << std::endl;
            player.get_mut<Health>()->decrease(5.f * it.delta_time());
            std::cout << "Player Health : " << player.get<Health>()->level << "\n";

            dirX = playerPosition->x - enemyPosition.x;
            dirY = playerPosition->y - enemyPosition.y;

            enemyPosition.angle = atan2(dirY, dirX) * 180 / M_PI ;

            dirX /= sqrt(dirX*dirX + dirY*dirY);
            dirY /= sqrt(dirX*dirX + dirY*dirY);

            enemyPosition.x +=  dirX * speed * it.delta_time();
            enemyPosition.y +=  dirY * speed * it.delta_time();
        }


    });


    auto clearScreenSys = world.system("clear").kind(flecs::PreUpdate)
    .iter([](flecs::iter& it)
    {
        SFMLWindow* window = it.world().get_mut<SFMLWindow>();
        window->window.clear(sf::Color::Black);
        window->window.setView(window->playerView);

    });

    auto drawSys = world.system<Position>().kind(flecs::OnUpdate)
    .iter([](flecs::iter it, Position *p)
    {
        SFMLWindow* window = it.world().get_mut<SFMLWindow>();

        for (int i : it) {
            Ball ball {p[i].x, p[i].y};
            Rectange rec {p[i].x, p[i].y, p[i].angle};
            window->window.draw(ball);
            window->window.draw(rec);
        }
    });

    auto displayScreenSys = world.system("display").kind(flecs::PostUpdate)
    .iter([](flecs::iter& it)
    {
        SFMLWindow* window = it.world().get_mut<SFMLWindow>();
        window->window.display();
    });



    SFMLWindow* window {world.get_mut<SFMLWindow>()};
    sf::Event& event {window->event};
    
    while (true)
    {
        while (window->window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window->window.close();
                std::cout << "Game Window closed\n";
                return;
            }
        }
        world.progress();
    }
    
}