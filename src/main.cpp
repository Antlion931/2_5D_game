#include "Resources.hpp"
#include "Renderer/Renderer.hpp"

#include <memory>
#include <iostream>

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

int main()
{
    std::unique_ptr<sf::RenderWindow> window = create_window();
    check_settings(*window);

    Renderer renderer(*window);
    sf::Event event;

    while (window->isOpen())
    {
        while (window->pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window->close();
            else if (event.type == sf::Event::Resized)
                renderer.resize(event.size.width, event.size.height);
            else if (event.type == sf::Event::KeyReleased)
            {
                if (event.key.code == sf::Keyboard::Z)
                    renderer.toggleSettings(Renderer::Settings::Draw3D);
                else if (event.key.code == sf::Keyboard::X)
                    renderer.toggleSettings(Renderer::Settings::Quiality);
            }
            else if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::W)
                    renderer.moveCamera({ 0.0f, 0.55f });
                else if (event.key.code == sf::Keyboard::S)
                    renderer.moveCamera({ 0.0f, -0.55f });
                else if (event.key.code == sf::Keyboard::A)
                    renderer.moveCamera({ -0.55f, 0.0f });
                else if (event.key.code == sf::Keyboard::D)
                    renderer.moveCamera({ 0.55f, 0.0f });
                else if (event.key.code == sf::Keyboard::Q)
                    renderer.rotateCamera({ -0.1f, 0.f, 0.f, 1.f});
                else if (event.key.code == sf::Keyboard::E)
                    renderer.rotateCamera({ 0.1f, 0.f, 0.f, 1.f});
            }
        }
        renderer.render();

        window->display();
    }

    return 0;
}