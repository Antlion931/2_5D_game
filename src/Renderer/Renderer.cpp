#include "Renderer/Renderer.hpp"

#include <flecs.h>
#include "Game/GameComponents.hpp"

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

    static const glm::mat4 proj = glm::ortho(200.f, -200.f, -200.f, 200.f, -1.f, 1.f);
    constexpr float PI = 3.14159265358979323846f;
}

Renderer::Renderer(GLFWwindow& window): m_window(window)
{
    glEnable(GL_DEPTH_TEST);

    const float vertices[] = {          // texture coords
        -100000.0f, -0.1f, -100000.0f,  -100000.0f, -100000.0f,
        -100000.0f, -0.1f, 100000.0f,   -100000.0f,  100000.0f,
        100000.0f, -0.1f, 100000.0f,     100000.0f,  100000.0f,
        100000.0f, -0.1f, -100000.0f,    100000.0f, -100000.0f
    };

    m_groundVertexBuffer = std::make_unique<VertexBuffer>(vertices, 20 * sizeof(float));
    m_groundVertexArray = std::make_unique<VertexArray>();

    VertexBufferLayout layout;
    layout.Push<float>(3);
    layout.Push<float>(2);

    m_groundVertexArray->AddBuffer(*m_groundVertexBuffer, layout);

    float skyboxVertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

    m_skyboxVertexBuffer = std::make_unique<VertexBuffer>(skyboxVertices, sizeof(skyboxVertices));
    m_skyboxVertexArray = std::make_unique<VertexArray>();

    VertexBufferLayout skyboxLayout;
    skyboxLayout.Push<float>(3);
    skyboxLayout.Push<float>(2);

    m_skyboxVertexArray->AddBuffer(*m_skyboxVertexBuffer, skyboxLayout);
}

void Renderer::render() noexcept
{
    // m_window.setActive(true);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (m_context.draw3D)
        draw3D();
    else
        draw2D();

    drawHUD();

    // m_window.setActive(false);
}

void Renderer::moveCamera(const glm::vec2& movement) noexcept
{
    // m_camera.position -= movement;

    const float angle = m_camera.rotation.x;
    const glm::mat4 rotation = glm::rotate(glm::mat4{1.0f}, angle, glm::vec3{0.0f, 1.0f, 0.0f});

    const glm::vec4 movement4 = rotation * glm::vec4{movement.x, 0.0f, movement.y, 1.0f};

    m_camera.position.x -= movement4.x;
    m_camera.position.y -= movement4.z;
}

void Renderer::setCamera(Position cp) noexcept
{
    m_camera.position = {cp.x, cp.y};
    m_camera.rotation = {cp.angle/360.f * 2.f * PI, 0.f, 0.f, 1.f};
}

void Renderer::rotateCamera(const glm::vec4& rotation) noexcept
{
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

void Renderer::load(const Level& level) noexcept
{
    const auto& walls = level.static_walls;

    const float groundHeight = 0.0f;

    for (int i = 0; i < 3; i++)
    {
        std::vector<float> vertices;
        std::vector<uint> indices;
        uint offset = 0;

        for (const auto& wall : walls)
        {
            if (wall.textureId != i)
                continue;

            const float distance = sqrtf(
                (wall.a.x - wall.b.x) * (wall.a.x - wall.b.x) +
                (wall.a.y - wall.b.y) * (wall.a.y - wall.b.y)
            );

            vertices.push_back(wall.a.x);
            vertices.push_back(wall.a.y);
            vertices.push_back(groundHeight);

            if (wall.textureMode == TextureMode::Stretch)
            {
                vertices.push_back(0.0f);
                vertices.push_back(0.0f);
            } else {
                vertices.push_back(0.0f);
                vertices.push_back(0.0f);
            }

            vertices.push_back(wall.b.x);
            vertices.push_back(wall.b.y);
            vertices.push_back(groundHeight);

            if (wall.textureMode == TextureMode::Stretch)
            {
                vertices.push_back(1.0f);
                vertices.push_back(0.0f);
            } else {
                vertices.push_back(distance / 4.f);
                vertices.push_back(0.0f);
            }

            vertices.push_back(wall.b.x);
            vertices.push_back(wall.b.y);
            vertices.push_back(wall.height);

            if (wall.textureMode == TextureMode::Stretch)
            {
                vertices.push_back(1.0f);
                vertices.push_back(1.0f);
            } else {
                vertices.push_back(distance / 4.f);
                vertices.push_back(wall.height / 4.f);
            }

            vertices.push_back(wall.a.x);
            vertices.push_back(wall.a.y);
            vertices.push_back(wall.height);

            if (wall.textureMode == TextureMode::Stretch)
            {
                vertices.push_back(0.0f);
                vertices.push_back(1.0f);
            } else {
                vertices.push_back(0.0f);
                vertices.push_back(wall.height / 4.f);
            }

            indices.push_back(offset + 0);
            indices.push_back(offset + 1);
            indices.push_back(offset + 2);

            indices.push_back(offset + 2);
            indices.push_back(offset + 3);
            indices.push_back(offset + 0);

            offset += 4;
        }

        m_walls[i].vertexBuffer =
            std::make_unique<VertexBuffer>(vertices.data(), vertices.size() * sizeof(float));
        m_walls[i].vertexArray =
            std::make_unique<VertexArray>();

        VertexBufferLayout layout;
        layout.Push<float>(3);
        layout.Push<float>(2);

        m_walls[i].vertexArray->AddBuffer(*m_walls[i].vertexBuffer, layout);

        m_walls[i].indexBuffer =
            std::make_unique<IndexBuffer>(indices.data(), indices.size());
    }
}

// TODO: Rewrite in modern OpenGL
void Renderer::draw3D() noexcept
{
    glClearColor(0.2f, 0.3f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    

    glm::vec3 cameraPos = glm::vec3(m_camera.position.x, 2.0f, m_camera.position.y);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    cameraFront = glm::rotate(glm::mat4{1.0f}, m_camera.rotation.x, cameraUp) * glm::vec4{cameraFront, 1.0f};


    glm::mat4 view = glm::lookAt(
        cameraPos,
        cameraPos + cameraFront,
        cameraUp
    );
    // view = glm::translate(view, glm::vec3(-m_camera.position.x, -2.0f, -m_camera.position.y));
    // view = glm::rotate(view, m_camera.rotation.x, glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 proj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 20000.0f);

    // Skybox

    glm::mat4 model = glm::mat4(1.0f);

    // model = glm::translate(model, cameraPos);
    model = glm::translate(model, glm::vec3(m_camera.position.x, -10.0f, m_camera.position.y));
    model = glm::scale(model, glm::vec3(10000.0f, 10000.0f, 10000.0f));

    m_skyboxShader.Bind();
    m_skyboxShader.SetUniformM("uModel", model);
    m_skyboxShader.SetUniformM("uView", view);
    m_skyboxShader.SetUniformM("uProjection", proj);
    m_skyboxShader.SetUniform("uTexture", 0);

    m_skyboxTexture.Bind();
    m_skyboxVertexArray->Bind();

    glDrawArrays(GL_TRIANGLES, 0, 36);
    // Ground


    m_groundShader.Bind();
    m_groundShader.SetUniformM("uModel", glm::mat4(1.0f));
    m_groundShader.SetUniformM("uView", view);
    m_groundShader.SetUniformM("uProjection", proj);
    m_groundShader.SetUniform("uTexture", 0);

    m_groundTexture.Bind();
    m_groundVertexArray->Bind();

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    m_groundVertexArray->Unbind();
    m_groundShader.Unbind();

    // Walls
    for (int i = 0; i < 3; i++)
    {
        auto& walls = m_walls[i];

        walls.vertexArray->Bind();
        walls.indexBuffer->Bind();

        m_wallShader.Bind();
        m_wallShader.SetUniformM("uModel", glm::mat4(1.0f));
        m_wallShader.SetUniformM("uView", view);
        m_wallShader.SetUniformM("uProjection", proj);
        m_wallShader.SetUniform("uTexture", 0);

        m_textures[i].Bind();

        glDrawElements(GL_TRIANGLES, walls.indexBuffer->GetCount(), GL_UNSIGNED_INT, nullptr);

        walls.indexBuffer->Unbind();
        walls.vertexArray->Unbind();
        m_wallShader.Unbind();
    }
    // m_wallShader.Bind();
    // m_wallShader.SetUniformM("uModel", glm::mat4(1.0f));
    // m_wallShader.SetUniformM("uView", view);
    // m_wallShader.SetUniformM("uProjection", proj);

    // m_wallVertexArray->Bind();
    // m_wallIndexBuffer->Bind();

    // glDrawElements(GL_TRIANGLES, m_wallIndexBuffer->GetCount(), GL_UNSIGNED_INT, nullptr);

    // m_wallIndexBuffer->Unbind();
    // m_wallVertexArray->Unbind();
    // m_wallShader.Unbind();
}


void Renderer::draw2D() noexcept
{
    // static std::unique_ptr<Shader> shader = std::make_unique<Shader>("resources/shaders/walls.glsl");
    // query->each([&](Walls& walls) {
    //     for (const auto& wall : walls.walls)
    //     {
    //         VertexBuffer vb(&wall.first, 2 * sizeof(wall.first));
    //         VertexBufferLayout layout;
    //         layout.Push<float>(2);

    //         VertexArray va;
    //         va.AddBuffer(vb, layout);
    //         va.Bind();
    //         shader->Bind();
    //         shader->SetUniform("u_color", 1.0f, 1.0f, 1.0f, 1.0f);
    //         shader->SetUniformM("u_projection", proj);
    //         shader->SetUniform("u_PlayerPos", m_camera.position.x, m_camera.position.y);
    //         shader->SetUniform("u_PlayerRot", m_camera.rotation.x);


    //         glDrawArrays(GL_LINES, 0, 2);

    //         va.Unbind();
    //         shader->Unbind();
    //     }
    // });

    
    // // Player
    // {
    //     const float playerLength = 2.0f;

    //     const float oX = m_camera.position.x;
    //     const float oY = m_camera.position.y;

    //     const float angle = m_camera.rotation.x;

    //     const float dX = playerLength * cos(angle);
    //     const float dY = playerLength * sin(angle);

    //     const float x1 = oX + dX;
    //     const float y1 = oY + dY;

    //     const float playerVertices[] = {
    //         oX, oY,
    //         x1, y1
    //     };

    //     VertexBuffer vb(playerVertices, 4 * sizeof(float));
    //     VertexBufferLayout layout;
    //     layout.Push<float>(2);

    //     VertexArray va;
    //     va.AddBuffer(vb, layout);

    //     shader->Bind();
    //     shader->SetUniform("u_color", 1.0f, 0.0f, 0.0f, 1.0f);
    //     shader->SetUniformM("u_projection", proj);
    //     va.Bind();

    //     glDrawArrays(GL_LINES, 0, 2);

    //     va.Unbind();
    //     shader->Unbind();
    // }

    //     const float fov = 90.f / 180.f * PI;
    // const float halfFov = fov / 2.f;

    // const uint rayCount = m_window.getSize().x / pixelsPerRay;

    // const float angleBetweenRays = fov / rayCount;

    //     for (uint i = 0; i < rayCount; i++)
    // {
    //     const float angle = m_camera.rotation.x - halfFov + i * angleBetweenRays + angleBetweenRays / 2.f;

    //     const float dX = cos(angle);
    //     const float dY = sin(angle);

    //     float distance = -1.f;
    //     glm::vec2 closestHit = {0.f, 0.f};

    //         query->each([&](Walls& walls) {
    //         for (const auto& wall : walls.walls)
    //         {
    //             const glm::vec2 p = m_camera.position;
    //             const glm::vec2 r = {dX, dY};

    //             const glm::vec2 q = wall.first;
    //             const glm::vec2 s = wall.second - wall.first;

    //             const float t = cross(q - p, s) / cross(r, s);

    //             if (t >= 0.f)
    //             {
    //                 const float u = cross(q - p, r) / cross(r, s);

    //                 if (u >= 0.f && u <= 1.f)
    //                 {
    //                     const glm::vec2 hit = q + u * s;

    //                     const float newDistance = glm::length(hit - m_camera.position);

    //                     if (distance == -1.f || newDistance < distance)
    //                     {
    //                         distance = newDistance;
    //                         closestHit = hit;
    //                     }
    //                 }
    //             }
    //         }
    //     });

    //         if (distance != -1.f)
    //     {
    //         const float vertices[] = {
    //             m_camera.position.x, m_camera.position.y,
    //             closestHit.x, closestHit.y
    //         };

    //         VertexBuffer vb(vertices, 4 * sizeof(float));
    //         VertexBufferLayout layout;
    //         layout.Push<float>(2);

    //         VertexArray va;
    //         va.AddBuffer(vb, layout);

    //         shader->Bind();
    //         shader->SetUniform("u_color", 0.0f, 1.0f, 0.0f, 1.0f);
    //         shader->SetUniformM("u_projection", proj);
    //         va.Bind();

    //         glDrawArrays(GL_LINES, 0, 2);

    //         va.Unbind();
    //         shader->Unbind();
    //     }
    // }
}
