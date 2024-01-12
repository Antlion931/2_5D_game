#include <iostream>
#include <format>
#include <fstream>
#include <ostream>
#include <iterator>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

#include <glm/glm.hpp>

#include "LevelEditor/Draw.hpp"
#include "LevelEditor/Input.hpp"
#include "LevelEditor/State.hpp"

#include "Common/Wall.hpp"

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

void load(std::vector<Wall>& walls, State& state)
{
    constexpr auto levels_path = std::string_view("levels/");

    std::string fname;

    std::cout << std::format("Enter filename ({}): ", state.filename) << std::flush;
    std::getline(std::cin, fname);

    if (!fname.empty())
        state.filename = fname;

    std::cout << std::format("Loading {}{}...\n", levels_path, state.filename);

    std::ifstream file(levels_path.data() + state.filename);

    if (!file.is_open())
    {
        log("Failed to open file " + state.filename);

        state.load = false;
        return;
    }

    size_t wall_count = 0;
    file >> wall_count;

    walls.clear();
    walls.reserve(wall_count);

    for (size_t i = 0; i < wall_count; ++i)
    {
        Wall wall;

        file >> wall.a.x >> wall.a.y >> wall.b.x >> wall.b.y >> wall.height >> wall.textureId;

        unsigned int textureMode;
        file >> textureMode;

        wall.textureMode = static_cast<TextureMode>(textureMode);

        walls.push_back(wall);
    }

    file.close();

    state.load = false;

    log("Loaded " + state.filename);
}

void save(const std::vector<Wall>& walls, State& state)
{
    constexpr auto levels_path = std::string_view("levels/");

    std::string fname;

    std::cout << std::format("Enter filename ({}): ", state.filename) << std::flush;
    std::getline(std::cin, fname);

    if (!fname.empty())
        state.filename = fname;

    std::cout << std::format("Saving to {}{}...\n", levels_path, state.filename);
    std::ofstream file(levels_path.data() + state.filename);

    if (!file.is_open())
    {
        log("Failed to open file " + state.filename);

        state.save = false;
        return;
    }

    file << std::format("{}", walls.size()) << std::endl;

    for (const Wall& wall : walls)
    {
        file << std::format("{} {} {} {} {} {} {}\n", 
            wall.a.x, wall.a.y, wall.b.x, wall.b.y, wall.height, wall.textureId, static_cast<unsigned int>(wall.textureMode));
    }

    file.close();

    state.save = false;

    log("Saved " + state.filename);
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

    std::vector<Wall> walls;

    while (state.isRunning)
    {
        handleInput(window, state);

        if (state.load)
            load(walls, state);

        if (state.save)
            save(walls, state);

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