#include <iostream>
#include <format>
#include <fstream>
#include <ostream>
#include <iterator>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "LevelEditor/Draw.hpp"
#include "LevelEditor/Input.hpp"
#include "LevelEditor/State.hpp"

#include "Common/Wall.hpp"
#include "Common/Level.hpp"

void create(std::vector<Wall>& walls, State& state)
{
    if (state.editorMode == EditorMode::Wall)
    {
        walls.push_back(
            Wall{
                .a = state.selectedV.value(),
                .b = state.focusedV,
                .height = state.height,
                .textureId = state.textureId,
                .textureMode = state.textureMode
            }
        );

        log(std::format("Added wall from {}x{} to {}x{}", 
            state.selectedV.value().x, 
            state.selectedV.value().y,
            state.focusedV.x,
            state.focusedV.y)
            );

        state.create = false;
        state.selectedV = std::nullopt;
    }
    else if (state.editorMode == EditorMode::Chunk)
    {
        // Not implemented
    }
}

void load(Level& level, State& state)
{
    std::string fileName;

    std::cout << std::format("Enter filename ({}): ", state.filename) << std::flush;
    std::getline(std::cin, fileName);

    if (!fileName.empty())
        state.filename = fileName;

    level = loadLevel(state.filename);

    state.load = false;
}

void save(const Level& level, State& state)
{
    std::string fileName;

    std::cout << std::format("Enter filename ({}): ", state.filename) << std::flush;
    std::getline(std::cin, fileName);

    if (!fileName.empty())
        state.filename = fileName;

    saveLevel(level, state.filename);

    state.save = false;
}

int main(int argc, char** argv)
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "Level Editor");
    State state;

    sf::View view = window.getDefaultView();
    
    std::cout << std::format("Window size: {}x{}, View size: {}x{}, View center: {}x{}\n", 
        window.getSize().x, window.getSize().y, view.getSize().x, view.getSize().y, view.getCenter().x, view.getCenter().y);

    view.setCenter(0.0f, 0.0f);
    view.setSize(window.getSize().x / state.zoom, window.getSize().y / state.zoom);
    window.setView(view);

    Level level;
    auto& walls = level.static_walls;

    while (state.isRunning)
    {
        handleInput(window, state);

        if (state.load)
            load(level, state);

        if (state.save)
            save(level, state);

        if (state.create)
            create(walls, state);

        if (state.undo)
        {
            if (!walls.empty())
            {
                walls.pop_back();
                log("Removed last wall");
            }

            state.undo = false;
        }

        window.clear(sf::Color::Black);

        if (state.drawGrid)
        {
            drawGrid(window, state);
        }

        drawWalls(window, walls);
        drawSelector(window, state);

        window.display();
    }

    return 0;
}