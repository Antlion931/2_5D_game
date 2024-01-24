#include "Resources.hpp"
#include "Renderer/Renderer.hpp"

#include <memory>
#include <iostream>

// std::unique_ptr<sf::RenderWindow> create_window(uint width = 800, uint height = 600, const std::string& title = "2_5D Renderer Demo", uint framerate = 60) noexcept
// {
//     sf::ContextSettings settings;

//     // Copied from previous project, idk if we keep those
//     settings.depthBits = 24;
//     settings.stencilBits = 8;
//     settings.antialiasingLevel = 4;
//     // Important settings
//     settings.majorVersion = 4;
//     settings.minorVersion = 6;
//     settings.attributeFlags = sf::ContextSettings::Core;

//     auto window = std::make_unique<sf::RenderWindow>(
//         sf::VideoMode(width, height),
//         title,
//         sf::Style::Default,
//         settings
//     );

//     window->setActive(true);
//     window->setVerticalSyncEnabled(true);
//     window->setFramerateLimit(framerate);

//     return std::move(window);
// }

// void check_settings(const sf::RenderWindow& window) noexcept
// {
//     sf::ContextSettings settings = window.getSettings();

//     std::cout << "Depth bits: " << settings.depthBits << std::endl;
//     std::cout << "Stencil bits: " << settings.stencilBits << std::endl;
//     std::cout << "Antialiasing level: " << settings.antialiasingLevel << std::endl;
//     std::cout << "Major version: " << settings.majorVersion << std::endl;
//     std::cout << "Minor version: " << settings.minorVersion << std::endl;
//     if (settings.attributeFlags & sf::ContextSettings::Core)
//         std::cout << "Core context" << std::endl;
//     else if (settings.attributeFlags & sf::ContextSettings::Debug)
//         std::cout << "Debug context" << std::endl;
//     else if (settings.attributeFlags & sf::ContextSettings::Default)
//         std::cout << "Default context" << std::endl;
//     else
//         std::cout << "Unknown context" << std::endl;
// }

void processInput(GLFWwindow* window, Renderer& renderer)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 0.15f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        renderer.moveCamera({0.0f, cameraSpeed});
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        renderer.moveCamera({0.0f, -cameraSpeed});
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        renderer.moveCamera({+cameraSpeed, 0.0f});
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        renderer.moveCamera({-cameraSpeed, 0.0f});
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        renderer.rotateCamera({0.1f, 0.0f, 0.0f, 0.00f});
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        renderer.rotateCamera({-0.1f, 0.0f, 0.0f, 0.00f});
}

int main()
{
    if(!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;

        int error_code = glfwGetError(nullptr);
        if (error_code == GLFW_PLATFORM_ERROR)
            std::cerr << "GLFW_PLATFORM_ERROR" << std::endl;
        else
            std::cerr << "Unknown error code" << std::endl;

        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
    #endif

    std::unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)> window(glfwCreateWindow(800, 600, "2_5D Renderer Demo", nullptr, nullptr), glfwDestroyWindow);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        return -1;
    }

    glfwMakeContextCurrent(window.get());

    if(!gladLoaderLoadGL())
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glfwSetFramebufferSizeCallback(window.get(), [](GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
    });


    Renderer renderer(*window);

    renderer.load(loadLevel("test.lvl"));

    while (!glfwWindowShouldClose(window.get()))
    {
        processInput(window.get(), renderer);

        renderer.render();

        glfwSwapBuffers(window.get());

        glfwPollEvents();
    }

    return 0;
}