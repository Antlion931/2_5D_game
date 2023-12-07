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
    SFMLWindow()
    {
        //window.create(sf::VideoMode{800, 600}, " Hello SFML!");
        // window.setFramerateLimit(60);
        playerView.setSize(800.f, 600.f);
        std::cout << "Singleton instance created." << std::endl;
    }
};

sf::RenderWindow SFMLWindow::window(sf::VideoMode{800, 600}, " Hello SFML!");


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


void run() 
{
    flecs::world world {};
    world.set<SFMLWindow>(SFMLWindow{});

    flecs::entity entity  {world.entity()};
    flecs::entity entity2 {world.entity()};
    flecs::entity entity3 {world.entity()};

    flecs::entity player {world.entity()};

    player.set<Position>({100, 100, 20}).set<Player>({});


    auto player_move = world.system<Player, Position>()
        .iter([](flecs::iter it, Player* player, Position* position)
        {
            float speed {30};

            for (int i : it)
            {
                bool isMoving {false};
                float deltaAngle {};

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
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
                {
                    position[i].angle += 20 * it.delta_time();
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
                {
                    position[i].angle -= 20 * it.delta_time();
                }
                
                if (isMoving)
                {
                    position[i].x += speed * cosf((position[i].angle + deltaAngle) * M_PI / 180) * it.delta_time(); 
                    position[i].y += speed * sinf((position[i].angle + deltaAngle) * M_PI / 180) * it.delta_time();        
                }
                
                it.world().get_mut<SFMLWindow>()->playerView.setCenter(position[i].x, position[i].y);
            }
            
        });


    entity.set(Position{200, 400, 10});
    entity2.set(Position{300, 500, 10});
    entity3.set(Position{520,440, 160});

    auto move_sys = world.system<Position>().kind(flecs::OnUpdate)
    .iter([](flecs::iter it, Position *p) {
        SFMLWindow* window = it.world().get_mut<SFMLWindow>();
        for (int i : it) {
            Ball ball {p[i].x, p[i].y};
            Rectange rec {p[i].x, p[i].y, p[i].angle};
            window->window.draw(ball);
            window->window.draw(rec);
        }
    });


    auto clear_screen_sys = world.system("clear").kind(flecs::PreUpdate).iter([](flecs::iter& it){
        SFMLWindow* window = it.world().get_mut<SFMLWindow>();
        window->window.clear(sf::Color::Black);
        window->window.setView(window->playerView);
    });


    auto display_screen_sys = world.system("display").kind(flecs::PostUpdate).iter([](flecs::iter& it){
        SFMLWindow* window = it.world().get_mut<SFMLWindow>();
        window->window.display();
    });


    while (true)
    {
        world.progress();
    }
    
}