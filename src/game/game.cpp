#include "game.hpp"
#include "collision.hpp"
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <flecs.h>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <math.h>
#include <Resources.hpp>
#include "Renderer/Renderer.hpp"
#include <memory>
#include <iostream>

#include "GameComponents.hpp"

std::unique_ptr<sf::RenderWindow> create_window(uint width = 800, uint height = 600, const std::string& title = "2_5D Renderer Demo", uint framerate = 60) noexcept
{
    sf::ContextSettings settings;

    // Copied from previous project, idk if we keep those
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 4;
    // Important settings
    settings.majorVersion = 4;
    settings.minorVersion = 6;
    // settings.attributeFlags = sf::ContextSettings::Core;

    auto window = std::make_unique<sf::RenderWindow>(
        sf::VideoMode(width, height),
        title,
        sf::Style::Default,
        settings
    );

    window->setActive(true);
    window->setVerticalSyncEnabled(true);
    window->setFramerateLimit(framerate);

    return std::move(window);
}

void check_settings(const sf::RenderWindow& window) noexcept
{
    sf::ContextSettings settings = window.getSettings();

    std::cout << "Depth bits: " << settings.depthBits << std::endl;
    std::cout << "Stencil bits: " << settings.stencilBits << std::endl;
    std::cout << "Antialiasing level: " << settings.antialiasingLevel << std::endl;
    std::cout << "Major version: " << settings.majorVersion << std::endl;
    std::cout << "Minor version: " << settings.minorVersion << std::endl;
    if (settings.attributeFlags & sf::ContextSettings::Core)
        std::cout << "Core context" << std::endl;
    else if (settings.attributeFlags & sf::ContextSettings::Debug)
        std::cout << "Debug context" << std::endl;
    else if (settings.attributeFlags & sf::ContextSettings::Default)
        std::cout << "Default context" << std::endl;
    else
        std::cout << "Unknown context" << std::endl;
}


class WindowLayer
{
public:
    static std::unique_ptr<sf::RenderWindow> window;
    static Renderer renderer;
    sf::View playerView;
    sf::Event event;
    WindowLayer() : playerView{{0.0, 0.0}, {800.f, 600.f}}
    {
        std::cout << "Singleton instance created." << std::endl;
    }
};

std::unique_ptr<sf::RenderWindow> WindowLayer::window = create_window(800, 600, "2_5D Renderer Demo", 60);
Renderer WindowLayer::renderer(*WindowLayer::window);

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
    check_settings(*WindowLayer::window);
    world.set<WindowLayer>(WindowLayer{});
    sf::Event event;

    flecs::entity enemy0 {world.entity()};
    flecs::entity enemy1 {world.entity()};
    flecs::entity enemy3 {world.entity()};

    std::vector<std::pair<glm::vec2, glm::vec2>> walls;

    flecs::entity wall12 {world.entity()};

    walls.clear();

    walls.push_back(std::make_pair(glm::vec2{30, 30}, glm::vec2{100, 130}));

    wall12.set(LineCollider{sf::Vector2f{30, 30}, sf::Vector2f{100, 130}}).set(Walls{walls});

    sf::CircleShape player_collider = sf::CircleShape();
    player_collider.setRadius(6);
    player_collider.setFillColor(sf::Color(255, 0, 0, 100));
    player_collider.setOutlineColor(sf::Color::Red);
    player_collider.setOrigin(sf::Vector2f{3, 3});
    flecs::entity player {world.entity("Player")};

    player.set<Position>({200, 380, 270}).set<Player>({}).set<Health>({100.f});

    player.set(CircleCollider{player_collider});

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
                    position[i].angle += 100 * it.delta_time();
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
                {
                    position[i].angle -= 100 * it.delta_time();
                }
                
                if (isMoving)
                {
                    position[i].x += speed * cosf((position[i].angle + deltaAngle) * M_PI / 180) * it.delta_time(); 
                    position[i].y += speed * sinf((position[i].angle + deltaAngle) * M_PI / 180) * it.delta_time();        
                }
                
                it.world().get_mut<WindowLayer>()->playerView.setCenter(position[i].x, position[i].y);

                auto wl = it.world().get_mut<WindowLayer>();

                wl->renderer.setCamera(position[i]);
            }
            
        });

    /*enemy0.set(Position{200, 400, 10}).set<Enemy>({}).set<Health>({}).add<Attacks>(player);
    enemy0.set(Collider{collider});
    enemy1.set(Position{300, 500, 10}).set<Enemy>({}).set<Health>({}).add<Attacks>(player);
    enemy1.set(Collider{collider});
    enemy3.set(Position{520,440, 160}).set<Enemy>({}).set<Health>({}).add<Attacks>(player);
    enemy3.set(Collider{collider});*/

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
        if (distance < 300.f)
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
        //SFMLWindow* window = it.world().get_mut<SFMLWindow>();
        //window->window.clear(sf::Color::Black);
        //window->window.setView(window->playerView);

    });

    auto drawSys = world.system<Position>().kind(flecs::OnUpdate)
    .iter([](flecs::iter it, Position *p)
    {
        //SFMLWindow* window = it.world().get_mut<SFMLWindow>();

        for (int i : it) {
            Ball ball {p[i].x, p[i].y};
            Rectange rec {p[i].x, p[i].y, p[i].angle};
            //window->window.draw(ball);
            //window->window.draw(rec);
        }
    });

    auto colider_debug_draw = world.system<ConvexShapeCollider>().kind(flecs::OnValidate)
    .iter([](flecs::iter it, ConvexShapeCollider *p) {
        //SFMLWindow* window = it.world().get_mut<SFMLWindow>();
        for (int i : it) {
            //window->window.draw(p[i].shape);
        }
    });


    WallsQuery walls_query = world.query<Walls>();

    auto drawScreenSys = world.system("draw").ctx(&walls_query).kind(flecs::PostUpdate)
    .iter([](flecs::iter& it)
    {
        WallsQuery *q = it.ctx<WallsQuery>();
        WindowLayer* wl = it.world().get_mut<WindowLayer>();
        wl->renderer.render(q);
    });

    auto collider_position_update = world.system<ConvexShapeCollider, Position>().kind(flecs::OnUpdate)
    .iter([](flecs::iter& it, ConvexShapeCollider* collider, Position* position){
        for (int i : it) {
            collider[i].shape.setPosition(position[i].x, position[i].y);
            collider[i].shape.setRotation(position[i].angle);

            //std::cout << position[i].x << " " << position[i].y << ", "<< position[i].angle << std::endl;
        }
    });

    CollisionQuery wall_query = world.query<ConvexShapeCollider, Walls>();

    auto collision_system = world.system<ConvexShapeCollider, Position>().without<Walls>().ctx(&wall_query).each([](flecs::iter& it, size_t i, ConvexShapeCollider& r1, Position& p1) {
            CollisionQuery *q = it.ctx<CollisionQuery>();
            flecs::entity e1 = it.entity(i);

            q->each([&](flecs::entity e2, ConvexShapeCollider& r2, Walls& w) {
                    auto position = positionOfAToResolveCollisionWithB(&r1, &r2);
                    p1.x = position.x;
                    p1.y = position.y;
                    //Update collider position as well so other collision will no respect that
                    r1.shape.setPosition(position.x, position.y);
            });
        });

    LinesWallsQuery line_wall_query = world.query<LineCollider, Walls>();

    auto line_circle_colider_debug = world.system<CircleCollider, Position>().ctx(&line_wall_query).kind(flecs::OnUpdate).each([](flecs::iter& it, size_t i, CircleCollider& c, Position& p) {
        LinesWallsQuery *q = it.ctx<LinesWallsQuery>();
        flecs::entity e1 = it.entity(i);

        c.shape.setPosition(p.x, p.y);

        q->each([&](flecs::entity e2, LineCollider& l, Walls& w) {
                auto resutl = l.collidesWith(&c);

                if (resutl.has_value())
                {
                    std::cout << "Collision" << std::endl;
                }
        });
    });

    auto clear_screen_sys = world.system("clear").kind(flecs::PreUpdate).iter([](flecs::iter& it){
        //SFMLWindow* window = it.world().get_mut<SFMLWindow>();

        //window->window.clear(sf::Color::Black);
        //window->window.setView(window->playerView);
    });

    auto display_screen_sys = world.system("display").kind(flecs::PostUpdate)
    .iter([](flecs::iter& it)
    {
        WindowLayer* wl = it.world().get_mut<WindowLayer>();
        wl->window->display();
    });

    WindowLayer* wl = world.get_mut<WindowLayer>();

    while (wl->window->isOpen())
    {
        while (wl->window->pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                wl->window->close();
            else if (event.type == sf::Event::Resized)
                wl->renderer.resize(event.size.width, event.size.height);
            else if (event.type == sf::Event::KeyReleased)
            {
                if (event.key.code == sf::Keyboard::Z)
                    wl->renderer.toggleSettings(Renderer::Settings::Draw3D);
                else if (event.key.code == sf::Keyboard::X)
                    wl->renderer.toggleSettings(Renderer::Settings::Quiality);
            }
        }
        world.progress();
    }
}
