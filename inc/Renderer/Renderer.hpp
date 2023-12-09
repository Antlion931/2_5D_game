#pragma once

#include <GL/glew.h>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <stdexcept>
#include <vector>

constexpr float PI = 3.14159265358979323846f;

class Renderer
{
    public:
        Renderer(sf::RenderWindow& window): m_window(window)
        {
            if (glewInit() != GLEW_OK)
                throw std::runtime_error("Failed to initialize GLEW");

            std::cout << "LOG: OpenGL Version: " << glGetString(GL_VERSION) << std::endl;   // TODO: Logging utility

        // TODO: Remove this, implement proper walls and level loading
            walls.push_back(
                {{-10.f, -10.f}, {10.f, -10.f}}
            );

            walls.push_back(
                {{-10.f,-10.f}, {-10.f, 10.f}}
            );

            walls.push_back(
                {{10.f, -10.f}, {10.f, 10.f}}
            );

            walls.push_back(
                {{-10.f, 10.f}, {10.f, 10.f}}
            );
        //
        }
        ~Renderer() {}

        void render() noexcept {
            m_window.setActive(true);

            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            if (m_context.draw3D)
                draw3D();
            else
                draw2D();

            drawHUD();

            m_window.setActive(false);
        }

        void moveCamera(const glm::vec2& movement) noexcept
        {
            m_camera.position += movement;
        }

        void rotateCamera(const glm::vec4& rotation) noexcept
        {
            if (rotation.y != 0.f || rotation.z != 0.f || rotation.w != 1.f)
            {
                std::cout << "LOG: Renderer::rotateCamera() - Rotation around Y and Z axis is not supported, ignoring" << std::endl;
                return;
            }

            m_camera.rotation += rotation;

            if (m_camera.rotation.x > 2.f * PI)
                m_camera.rotation.x -= 2.f * PI;
            else if (m_camera.rotation.x < 0.f)
                m_camera.rotation.x += 2.f * PI;
        }

        void resize(uint width, uint height) noexcept
        {
            glViewport(0, 0, width, height);
        }

        enum class Settings : uint
        {
            Draw3D = 1 << 0
        };

        void toggleSettings(Settings settings) noexcept
        {
            if (static_cast<uint>(settings) & static_cast<uint>(Settings::Draw3D))
            {
                m_context.draw3D = !m_context.draw3D;
            }
        }

        int getSettings(Settings settings) const noexcept
        {
            // If more than one setting is passed, return -1
            if (static_cast<uint>(settings) & (static_cast<uint>(settings) - 1))
            {
                std::cout << "LOG: Renderer::getSettings() - More than one setting passed, returning -1" << std::endl;
                return -1;
            }

            if (static_cast<uint>(settings) & static_cast<uint>(Settings::Draw3D))
            {
                return static_cast<int>(m_context.draw3D);
            }

            std::cout << "LOG: Renderer::getSettings() - Invalid setting passed, returning -1" << std::endl;
            return -1;
        }

    private:
        sf::RenderWindow& m_window;

        std::vector<std::pair<glm::vec2, glm::vec2>> walls = {};

        struct {
            bool draw3D = false;
        } m_context;

        struct {
            glm::vec2 position = {0.0f, 0.0f};
            glm::vec4 rotation = {0.0f, 0.0f, 0.0f, 1.0f};
        } m_camera;

        // Proper 3D rendering
        void draw3D() noexcept
        {
            // TODO
        }

        // Draw in 2D for debugging purposes
        void draw2D() noexcept
        {

        }

        // Do later when Prototype is done
        void drawHUD() noexcept
        {
            // TODO
        }

};
