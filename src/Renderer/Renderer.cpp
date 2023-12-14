#include "Renderer/Renderer.hpp"

#include <flecs.h>
#include "GameComponents.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <vector>

#include "Renderer/VertexBuffer.hpp"
#include "Renderer/VertexBufferLayout.hpp"
#include "Renderer/VertexArray.hpp"
#include "Renderer/IndexBuffer.hpp"

namespace {
    constexpr float cross(const glm::vec2& a, const glm::vec2& b) noexcept
    {
        return a.x * b.y - a.y * b.x;
    }

    static const glm::mat4 proj = glm::ortho(-200.f, 200.f, -200.f, 200.f, -1.f, 1.f);
    constexpr float PI = 3.14159265358979323846f;
}

Renderer::Renderer(sf::RenderWindow& window): m_window(window)
    {
        if (glewInit() != GLEW_OK)
            throw std::runtime_error("Failed to initialize GLEW");

        std::cout << "LOG: OpenGL Version: " << glGetString(GL_VERSION) << std::endl;   // TODO: Logging utility
    }

void Renderer::render(WallsQuery* query) noexcept
{
    m_window.setActive(true);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (m_context.draw3D)
        draw3D(query);
    else
        draw2D(query);

    drawHUD();

    m_window.setActive(false);
}

void Renderer::moveCamera(const glm::vec2& movement) noexcept
{
    const float angle = m_camera.rotation.x - PI / 2.f;

    const float dX = movement.x * cos(angle) - movement.y * sin(angle);
    const float dY = movement.x * sin(angle) + movement.y * cos(angle);

    m_camera.position += glm::vec2(dX, dY);
}

void Renderer::setCamera(Position cp) noexcept
{
    m_camera.position = {cp.x, cp.y};
    m_camera.rotation = {cp.angle/360.f * 2.f * PI, 0.f, 0.f, 1.f};
}

void Renderer::rotateCamera(const glm::vec4& rotation) noexcept
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

void Renderer::resize(uint width, uint height) noexcept
{
    glViewport(0, 0, width, height);
}

void Renderer::toggleSettings(Settings settings) noexcept
{
    if (static_cast<uint>(settings) & static_cast<uint>(Settings::Draw3D))
    {
        m_context.draw3D = !m_context.draw3D;
    }
    if (static_cast<uint>(settings) & static_cast<uint>(Settings::Quiality))
    {
        m_context.quality = static_cast<Quality>(static_cast<uint>(m_context.quality) << 1);

        if (static_cast<uint>(m_context.quality) > static_cast<uint>(Quality::Ultra))
            m_context.quality = Quality::VLow;

        switch (m_context.quality)
        {
            case Quality::VLow:
                pixelsPerRay = 100;
                break;
            case Quality::Low:
                pixelsPerRay = 50;
                break;
            case Quality::Medium:
                pixelsPerRay = 25;
                break;
            case Quality::High:
                pixelsPerRay = 10;
                break;
            case Quality::VHigh:
                pixelsPerRay = 5;
                break;
            case Quality::Ultra:
                pixelsPerRay = 1;
                break;
        }
    }
}

[[nodiscard]] int Renderer::getSettings(Settings settings) const noexcept
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

// TODO: Rewrite in modern OpenGL
void Renderer::draw3D(WallsQuery* query) noexcept
{
    const float fov = 90.f / 180.f * PI;
    const float halfFov = fov / 2.f;

    const uint rayCount = m_window.getSize().x / pixelsPerRay;

    const float angleBetweenRays = fov / rayCount;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.f, m_window.getSize().x, m_window.getSize().y, 0.f, -1.f, 1.f);

    // Celling
    glBegin(GL_QUADS);
        glColor3f(0.6f, 0.6f, 0.6f);
        glVertex2f(0.f, 0.f);
        glVertex2f(m_window.getSize().x, 0.f);
        glVertex2f(m_window.getSize().x, m_window.getSize().y / 2.f);
        glVertex2f(0.f, m_window.getSize().y / 2.f);
    glEnd();

    // Floor
    glBegin(GL_QUADS);
        glColor3f(0.3f, 0.3f, 0.3f);
        glVertex2f(0.f, m_window.getSize().y / 2.f);
        glVertex2f(m_window.getSize().x, m_window.getSize().y / 2.f);
        glVertex2f(m_window.getSize().x, m_window.getSize().y);
        glVertex2f(0.f, m_window.getSize().y);
    glEnd();

    for (uint i = 0; i < rayCount; i++)
    {
        const float angle = m_camera.rotation.x - halfFov + i * angleBetweenRays + angleBetweenRays / 2.f;

        const float dX = cos(angle);
        const float dY = sin(angle);

        float distance = -1.f;
        glm::vec2 closestHit = {0.f, 0.f};


        query->each([&](Walls& walls) {
            for (const auto& wall : walls.walls)
            {
                const glm::vec2 p = m_camera.position;
                const glm::vec2 r = {dX, dY};

                const glm::vec2 q = wall.first;
                const glm::vec2 s = wall.second - wall.first;

                const float t = cross(q - p, s) / cross(r, s);

                if (t >= 0.f)
                {
                    const float u = cross(q - p, r) / cross(r, s);

                    if (u >= 0.f && u <= 1.f)
                    {
                        const glm::vec2 hit = q + u * s;

                        const float newDistance = glm::length(hit - m_camera.position);

                        if (distance == -1.f || newDistance < distance)
                        {
                            distance = newDistance;
                            closestHit = hit;
                        }
                    }
                }
            }
        });

        if (distance != -1.f)
        {
            auto pow2([](float x) { return x * x; });
            auto clamp([](float x, float min, float max) { return x < min ? min : (x > max ? max : x); });
            float color = pow2(1.f - clamp(distance / 150.f, 0.f, 1.f));
                    
            const float height = m_window.getSize().y;
            const float center = m_window.getSize().x / 2.f;
            const float wallHeight = height / distance * 8.f;
            glBegin(GL_QUADS);
                glOrtho(0.f, m_window.getSize().x, m_window.getSize().y, 0.f, -1.f, 1.f);
                glColor3f(color, 0.f, 0.f);
                glVertex2f(i * pixelsPerRay, height / 2.f - wallHeight / 2.f);
                glVertex2f(i * pixelsPerRay + pixelsPerRay, height / 2.f - wallHeight / 2.f);
                glVertex2f(i * pixelsPerRay + pixelsPerRay, height / 2.f + wallHeight / 2.f);
                glVertex2f(i * pixelsPerRay, height / 2.f + wallHeight / 2.f);
            glEnd();
        }
    }
}


void Renderer::draw2D(WallsQuery *query) noexcept
{
    query->each([&](Walls& walls) {
        for (const auto& wall : walls.walls)
        {
            VertexBuffer vb(&wall.first, 2 * sizeof(wall.first));
            VertexBufferLayout layout;
            layout.Push<float>(2);

            VertexArray va;
            va.AddBuffer(vb, layout);
            std::cout << "1" << std::endl;

            shader->Bind(); // FIXME: You can no bind in a query
            std::cout << "2" << std::endl;
            shader->SetUniform("u_color", 1.0f, 1.0f, 1.0f, 1.0f);
            shader->SetUniformM("u_projection", proj);
            va.Bind();


            glDrawArrays(GL_LINES, 0, 2);

            va.Unbind();
            shader->Unbind();
        }
    });


    // Player
    {
        const float playerLength = 2.0f;

        const float oX = m_camera.position.x;
        const float oY = m_camera.position.y;

        const float angle = m_camera.rotation.x;

        const float dX = playerLength * cos(angle);
        const float dY = playerLength * sin(angle);

        const float x1 = oX + dX;
        const float y1 = oY + dY;

        const float playerVertices[] = {
            oX, oY,
            x1, y1
        };

        VertexBuffer vb(playerVertices, 4 * sizeof(float));
        VertexBufferLayout layout;
        layout.Push<float>(2);

        VertexArray va;
        va.AddBuffer(vb, layout);

        shader->Bind();
        shader->SetUniform("u_color", 1.0f, 0.0f, 0.0f, 1.0f);
        shader->SetUniformM("u_projection", proj);
        va.Bind();

        glDrawArrays(GL_LINES, 0, 2);

        va.Unbind();
        shader->Unbind();
    }

    const float fov = 90.f / 180.f * PI;
    const float halfFov = fov / 2.f;

    const uint rayCount = m_window.getSize().x / pixelsPerRay;

    const float angleBetweenRays = fov / rayCount;

    for (uint i = 0; i < rayCount; i++)
    {
        const float angle = m_camera.rotation.x - halfFov + i * angleBetweenRays + angleBetweenRays / 2.f;

        const float dX = cos(angle);
        const float dY = sin(angle);

        float distance = -1.f;
        glm::vec2 closestHit = {0.f, 0.f};

        query->each([&](Walls& walls) {
            for (const auto& wall : walls.walls)
            {
                const glm::vec2 p = m_camera.position;
                const glm::vec2 r = {dX, dY};

                const glm::vec2 q = wall.first;
                const glm::vec2 s = wall.second - wall.first;

                const float t = cross(q - p, s) / cross(r, s);

                if (t >= 0.f)
                {
                    const float u = cross(q - p, r) / cross(r, s);

                    if (u >= 0.f && u <= 1.f)
                    {
                        const glm::vec2 hit = q + u * s;

                        const float newDistance = glm::length(hit - m_camera.position);

                        if (distance == -1.f || newDistance < distance)
                        {
                            distance = newDistance;
                            closestHit = hit;
                        }
                    }
                }
            }
        });

        if (distance != -1.f)
        {
            const float vertices[] = {
                m_camera.position.x, m_camera.position.y,
                closestHit.x, closestHit.y
            };

            VertexBuffer vb(vertices, 4 * sizeof(float));
            VertexBufferLayout layout;
            layout.Push<float>(2);

            VertexArray va;
            va.AddBuffer(vb, layout);

            shader->Bind();
            shader->SetUniform("u_color", 0.0f, 1.0f, 0.0f, 1.0f);
            shader->SetUniformM("u_projection", proj);
            va.Bind();

            glDrawArrays(GL_LINES, 0, 2);

            va.Unbind();
            shader->Unbind();
        }
    }
}
