#include "game.hpp"
#include "collision.hpp"
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

struct Wall {
    bool a;
};

using CollisionQuery = flecs::query<Collider, Wall>;

void run() 
{
    flecs::world world {};
    world.set<SFMLWindow>(SFMLWindow{});

    flecs::entity entity  {world.entity()};
    flecs::entity entity2 {world.entity()};
    flecs::entity entity3 {world.entity()};

    flecs::entity wall {world.entity()};
    auto collider = sf::ConvexShape{4};
    collider.setPoint(0, sf::Vector2f{0, 0});
    collider.setPoint(1, sf::Vector2f{0, 400});
    collider.setPoint(2, sf::Vector2f{40, 400});
    collider.setPoint(3, sf::Vector2f{40, 0});
    collider.setFillColor(sf::Color(0, 255, 0, 100));
    collider.setOutlineColor(sf::Color::Green);
    collider.setOrigin(sf::Vector2f{20, 200});
    collider.setPosition(0, 0);

    wall.set(Collider{collider}).set<Wall>({});

    flecs::entity player {world.entity()};

    player.set<Position>({100, 100, 20}).set<Player>({});
    collider = sf::ConvexShape{4};
    collider.setPoint(0, sf::Vector2f{0, 0});
    collider.setPoint(1, sf::Vector2f{0, 40});
    collider.setPoint(2, sf::Vector2f{40, 40});
    collider.setPoint(3, sf::Vector2f{40, 0});
    collider.setFillColor(sf::Color(255, 0, 0, 100));
    collider.setOutlineColor(sf::Color::Red);
    collider.setOrigin(sf::Vector2f{20, 20});

    player.set(Collider{collider});

    auto player_move = world.system<Player, Position>()
        .iter([](flecs::iter it, Player* player, Position* position)
        {
            float speed {60};

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

    auto colider_debug_draw = world.system<Collider>().kind(flecs::OnValidate)
    .iter([](flecs::iter it, Collider *p) {
        SFMLWindow* window = it.world().get_mut<SFMLWindow>();
        for (int i : it) {
            window->window.draw(p[i].shape);
        }
    });

    auto collider_position_update = world.system<Collider, Position>().kind(flecs::OnUpdate)
    .iter([](flecs::iter& it, Collider* collider, Position* position){
        for (int i : it) {
            collider[i].shape.setPosition(position[i].x, position[i].y);
            collider[i].shape.setRotation(position[i].angle);

            //std::cout << position[i].x << " " << position[i].y << ", "<< position[i].angle << std::endl;
        }
    });

    CollisionQuery wall_query = world.query<Collider, Wall>();

    auto collision_system = world.system<Collider, Position>().without<Wall>().ctx(&wall_query).each([](flecs::iter& it, size_t i, Collider& r1, Position& p1) {
            CollisionQuery *q = it.ctx<CollisionQuery>();
            flecs::entity e1 = it.entity(i);

            q->each([&](flecs::entity e2, Collider& r2, Wall& w) {
                    auto position = r1.updatePositionToResolveCollision(&r2);
                    p1.x = position.x;
                    p1.y = position.y;
            });
        });

    auto clear_screen_sys = world.system("clear").kind(flecs::PreUpdate).iter([](flecs::iter& it){
        SFMLWindow* window = it.world().get_mut<SFMLWindow>();

        window->window.clear(sf::Color::Black);
        window->window.setView(window->playerView);
        
        // Process events
        sf::Event event;
        while (window->window.pollEvent(event))
        {
            // Close window: exit
            if (event.type == sf::Event::Closed)
                window->window.close();
        }
    });


    auto display_screen_sys = world.system("display").kind(flecs::PostUpdate).iter([](flecs::iter& it){
        SFMLWindow* window = it.world().get_mut<SFMLWindow>();
        window->window.display();
    });


    while (SFMLWindow::window.isOpen())
    {
        world.progress();
    }
    
}
