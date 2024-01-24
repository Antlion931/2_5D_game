#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "Renderer/Shader.hpp"
#include "Renderer/VertexArray.hpp"
#include "Renderer/VertexBuffer.hpp"
#include "Renderer/IndexBuffer.hpp"
#include "Renderer/Texture.hpp"

#include "Game/GameComponents.hpp"

#include "Common/Level.hpp"
#include "Common/Log.hpp"

class Renderer
{
    public:
        Renderer(GLFWwindow& window);
        ~Renderer() = default;

        void render() noexcept;
        void moveCamera(const glm::vec2& movement) noexcept;
        void setCamera(Position cameraPosition) noexcept;
        void rotateCamera(const glm::vec4& rotation) noexcept;
        void resize(uint width, uint height) noexcept;
        void load(const Level& level) noexcept;

        enum class Settings : uint
        {
            Draw3D = 1 << 0,
            Quiality = 1 << 1
        };

        void toggleSettings(Settings settings) noexcept;
        [[nodiscard]] int getSettings(Settings settings) const noexcept;

    private:
        // Static walls
        struct WallType {
            uint texture;
            std::unique_ptr<VertexBuffer> vertexBuffer;
            std::unique_ptr<VertexArray> vertexArray;
            std::unique_ptr<IndexBuffer> indexBuffer;
        };

        std::array<WallType, 3> m_walls {};

        Shader m_wallShader = Shader("resources/shaders/wall3d.glsl");
        // std::unique_ptr<VertexBuffer> m_wallVertexBuffer;
        // std::unique_ptr<VertexArray> m_wallVertexArray;
        // std::unique_ptr<IndexBuffer> m_wallIndexBuffer;
        // Ground and ceiling
        Texture m_groundTexture = Texture("resources/textures/ground.jpg");
        Shader m_groundShader = Shader("resources/shaders/ground.glsl");
        std::unique_ptr<VertexBuffer> m_groundVertexBuffer;
        std::unique_ptr<VertexArray> m_groundVertexArray;
        //
        Texture m_skyboxTexture = Texture("resources/textures/skybox.png");
        Shader m_skyboxShader = Shader("resources/shaders/skybox.glsl");
        std::unique_ptr<VertexBuffer> m_skyboxVertexBuffer;
        std::unique_ptr<VertexArray> m_skyboxVertexArray;
        std::unique_ptr<IndexBuffer> m_skyboxIndexBuffer;

        const std::array<Texture, 3> m_textures = {
            Texture("resources/textures/building2.jpg"),
            Texture("resources/textures/wood.jpg"),
            Texture("resources/textures/bricks.jpg")
        };

        GLFWwindow& m_window;

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
            glm::vec2 position = {-0.0f, -0.0f};
            glm::vec4 rotation = {0.0f, 0.0f, 0.0f, 1.0f};
        } m_camera;

        glm::mat4 proj_3d;
        uint pixelsPerRay = 1;

        // Proper 3D rendering
        void draw3D() noexcept;

        // Draw in 2D for debugging purposes
        void draw2D() noexcept;

        // Do later when Prototype is done
        void drawHUD() noexcept
        {
            // TODO
        }
};
