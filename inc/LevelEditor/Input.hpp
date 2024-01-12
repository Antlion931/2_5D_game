#pragma once

#include <cmath>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

#include "LevelEditor/State.hpp"
#include "Common/Log.hpp"

sf::Vector2f snapToGrid(sf::Vector2f v, State& state)
{
    const float gridCellSize = state.gridCellSize;

    const float x = std::round(v.x / gridCellSize) * gridCellSize;
    const float y = std::round(v.y / gridCellSize) * gridCellSize;

    return sf::Vector2f(x, y);
}

void changeZoom(sf::RenderWindow& window, float zoom)
{
    sf::View view = window.getView();
    view.setSize(window.getSize().x / zoom, window.getSize().y / zoom);
    window.setView(view);
}

void changeViewCenter(sf::RenderWindow& window, sf::Vector2f viewCenter)
{
    sf::View view = window.getView();
    view.setCenter(viewCenter);
    window.setView(view);
}

template <sf::Keyboard::Key key>
void KeyPressed(State& state);

template <>
void KeyPressed<sf::Keyboard::RBracket>(State& state)
{
    if (state.editorMode != EditorMode::Wall)
        return;

    if (state.ctrl)
        log("textureMode = " + textureModeToString(++state.textureMode));
    else if (state.shift)
        log("textureId = " + std::to_string(++state.textureId));
    else
        log("height = " + std::to_string(++state.height));
}

template <>
void KeyPressed<sf::Keyboard::LBracket>(State& state)
{
    if (state.editorMode != EditorMode::Wall)
        return;

    if (state.ctrl)
        log("textureMode = " + textureModeToString(--state.textureMode));
    else if (state.shift)
        log("textureId = " + std::to_string(--state.textureId));
    else
        log("height = " + std::to_string(--state.height));
}

void handleInput(sf::RenderWindow& window, State& state)
{
    sf::Event event;
    while (window.pollEvent(event))
        if (event.type == sf::Event::Closed)
        {
            state.isRunning = false;
        }
        else if (event.type == sf::Event::KeyPressed)
        {
            switch (event.key.code)
            {
                case sf::Keyboard::Escape:
                    state.isRunning = false;
                    break;

                case sf::Keyboard::LShift:
                    state.shift = true;
                    break;

                case sf::Keyboard::LControl:
                    state.ctrl = true;
                    break;

                case sf::Keyboard::G:
                    state.drawGrid = !state.drawGrid;
                    break;

                case sf::Keyboard::C:
                    state.selectedV = std::nullopt;
                    break;

                case sf::Keyboard::S:
                    if (state.ctrl && !state.save)
                        state.save = true;
                    else
                        state.snapToGrid = !state.snapToGrid;
                    break;

                case sf::Keyboard::L:
                    if (state.ctrl && !state.load)
                        state.load = true;
                    break;

                case sf::Keyboard::U:
                    state.undo = true;
                    break;

                case sf::Keyboard::M:
                    // state.editorMode = state.editorMode == EditorMode::Wall ? EditorMode::Chunk : EditorMode::Wall;
                    // log("editorMode = " + std::string(state.editorMode == EditorMode::Wall ? "Chunk" : "Wall"));
                    break;

                case sf::Keyboard::RBracket:
                    KeyPressed<sf::Keyboard::RBracket>(state);
                    break;
                case sf::Keyboard::LBracket:
                    KeyPressed<sf::Keyboard::LBracket>(state);
                    break;

                default:
                    break;
            }
        }
        else if (event.type == sf::Event::KeyReleased)
        {
            switch (event.key.code)
            {
                case sf::Keyboard::LShift:
                    state.shift = false;
                    break;

                case sf::Keyboard::LControl:
                    state.ctrl = false;
                    break;

                default:
                    break;
            }
        }
        else if (event.type == sf::Event::MouseMoved)
        {
            static sf::Vector2f lastMousePos = sf::Vector2f(0.0f, 0.0f);

            float x = (event.mouseMove.x / state.zoom) + state.viewCenter.x - (window.getSize().x / (2.0f * state.zoom));
            float y = (event.mouseMove.y / state.zoom) + state.viewCenter.y - (window.getSize().y / (2.0f * state.zoom));

            if (state.snapToGrid)
                state.focusedV = snapToGrid(sf::Vector2f{x,y}, state);
            else
                state.focusedV = sf::Vector2f{x,y};

            if (state.isDragging)
            {
                state.viewCenter.x += (lastMousePos.x - event.mouseMove.x) / state.zoom;
                state.viewCenter.y += (lastMousePos.y - event.mouseMove.y) / state.zoom;
                changeViewCenter(window, state.viewCenter);
            }

            lastMousePos = sf::Vector2f(event.mouseMove.x, event.mouseMove.y);
        }
        else if (event.type == sf::Event::MouseWheelScrolled)
        {
            const float amount = event.mouseWheelScroll.delta;
            const float zoomFactor = 1.1f;

            state.zoom *= (amount > 0) ? zoomFactor : 1.0f / zoomFactor;
            changeZoom(window, state.zoom);

            log("zoom = " + std::to_string(state.zoom));
        }
        else if (event.type == sf::Event::MouseButtonPressed)
        {
            if (event.mouseButton.button == sf::Mouse::Left)
            {
                if (state.selectedV.has_value())
                {
                    if (state.selectedV.value() == state.focusedV)
                        state.selectedV = std::nullopt;
                    else
                        state.create = true;
                    return;
                }
                else 
                {
                    state.selectedV = state.focusedV;
                }
            }
            else if (event.mouseButton.button == sf::Mouse::Right)
            {
                state.isDragging = true;
            }
        }
        else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Right)
        {
            state.isDragging = false;
        }
}


                    // if (state.editorMode == EditorMode::Wall){
                    //     if (state.ctrl)
                    //         log("textureMode = " + textureModeToString(++state.textureMode));
                    //     else
                    //         log("textureId = " + std::to_string(++state.textureId));
                    // } else if (state.editorMode == EditorMode::Chunk){
                    //     if (state.shift && state.ctrl) {
                    //         log("selectedChunk = " + std::to_string(++state.selectedChunk));
                    //         state.chunk_update = true;
                    //     }
                    //     else if (state.shift)
                    //         log("isCeilingVisible = " + std::to_string((state.isCeilingVisible = !state.isCeilingVisible)));
                    //     else if (state.ctrl)
                    //         log("ceilingHeight = " + std::to_string(++state.ceilingHeight));
                    //     else
                    //         log("floorHeight = " + std::to_string(++state.floorHeight));
                    // }
                    // break;