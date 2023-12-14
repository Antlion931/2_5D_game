#pragma once

#include <GL/glew.h>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include "Renderer/Shader.hpp"
#include "GameComponents.hpp"

class Renderer
{
    public:
        Renderer(sf::RenderWindow& window);
        ~Renderer() = default;

        void render(WallsQuery* query) noexcept;
        void moveCamera(const glm::vec2& movement) noexcept;
        void setCamera(Position cameraPosition) noexcept;
        void rotateCamera(const glm::vec4& rotation) noexcept;
        void resize(uint width, uint height) noexcept;

        enum class Settings : uint
        {
            Draw3D = 1 << 0,
            Quiality = 1 << 1
        };

        void toggleSettings(Settings settings) noexcept;
        [[nodiscard]] int getSettings(Settings settings) const noexcept;

    private:
        // TODO remove later
        Shader* shader = nullptr;
        Shader* player_shader = nullptr;
        Shader* shader3d = nullptr;
        //

        sf::RenderWindow& m_window;

        enum class Quality : uint
        {
            VLow = 1 << 0,
            Low = 1 << 1,
            Medium = 1 << 2,
            High = 1 << 3,
            VHigh = 1 << 4,
            Ultra = 1 << 5
        };

        struct {
            bool draw3D = true;
            Quality quality = Quality::Ultra;
        } m_context;

        struct {
            glm::vec2 position = {-1.0f, -1.0f};
            glm::vec4 rotation = {0.0f, 0.0f, 0.0f, 1.0f};
        } m_camera;

        glm::mat4 proj_3d;
        uint pixelsPerRay = 1;

        // Proper 3D rendering
        void draw3D(WallsQuery *query) noexcept;

        // Draw in 2D for debugging purposes
        void draw2D(WallsQuery *query) noexcept;

        // Do later when Prototype is done
        void drawHUD() noexcept
        {
            // TODO
        }
};
