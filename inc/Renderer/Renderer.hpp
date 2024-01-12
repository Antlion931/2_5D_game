#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "Renderer/Shader.hpp"
#include "Renderer/VertexArray.hpp"
#include "Renderer/VertexBuffer.hpp"
#include "Renderer/IndexBuffer.hpp"

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
        std::unique_ptr<Shader> m_wallShader;
        std::unique_ptr<VertexBuffer> m_wallVertexBuffer;
        std::unique_ptr<VertexArray> m_wallVertexArray;
        std::unique_ptr<IndexBuffer> m_wallIndexBuffer;
        // Ground and ceiling
        std::unique_ptr<Shader> m_groundShader;
        std::unique_ptr<VertexBuffer> m_groundVertexBuffer;
        std::unique_ptr<VertexArray> m_groundVertexArray;
        //

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
