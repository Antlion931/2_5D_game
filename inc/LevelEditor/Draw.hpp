#pragma once

#include <vector>
#include <optional>
#include <cmath>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/VertexArray.hpp>

#include "LevelEditor/State.hpp"
#include "Common/Log.hpp"


void drawDot(sf::RenderWindow& window, const sf::Vector2f& v, const sf::Color& color, float size = 0.5f)
{
    sf::CircleShape circle(size);

    circle.setFillColor(color);
    circle.setPosition(v.x - size / 2.f, v.y - size / 2.f);

    window.draw(circle);
}

void drawSelectedV(sf::RenderWindow& window, const State& state)
{
    if (state.selectedV.has_value())
    {
        drawDot(window, state.selectedV.value(), sf::Color::Green);
    }
}

void drawFocusedV(sf::RenderWindow& window, const State& state)
{
    drawDot(window, state.focusedV, sf::Color::Red);
}

void drawCenter(sf::RenderWindow& window, const State& state)
{
    drawDot(window, sf::Vector2f(0.0f, 0.0f), sf::Color::White);
}

void drawConnection(sf::RenderWindow& window, const State& state)
{
    if (state.selectedV.has_value())
    {
        sf::VertexArray line(sf::Lines, 2);

        line[0].position = state.selectedV.value();
        line[1].position = state.focusedV;

        line[0].color = sf::Color(255, 255, 255, 200);
        line[1].color = sf::Color(255, 255, 255, 200);

        window.draw(line);
    }
}

void drawSelector(sf::RenderWindow& window, const State& state)
{
    drawCenter(window, state);
    drawConnection(window, state);
    drawSelectedV(window, state);
    drawFocusedV(window, state);
}

void drawGrid(sf::RenderWindow& window, const State& state)
{
    const sf::Vector2f viewSize = window.getView().getSize();
    const float viewWidth = viewSize.x;
    const float viewHeight = viewSize.y;

    const sf::Color gridColor = sf::Color(255, 255, 255, 30);
    const float gridCellSize = state.gridCellSize;
    const float gridWidth = state.gridWidth;

    const sf::Vector2f viewCenter = window.getView().getCenter();
    const sf::Vector2f gridOrigin = sf::Vector2f(
        std::floor(viewCenter.x / gridCellSize) * gridCellSize,
        std::floor(viewCenter.y / gridCellSize) * gridCellSize
    );

    const uint gridFactorX = std::ceil(viewWidth / (2.0f * gridCellSize)) + 1;

    const float gridLeft = gridOrigin.x - gridCellSize * gridFactorX;
    const float gridRight = gridOrigin.x + gridCellSize * gridFactorX;

    const uint gridFactorY = std::ceil(viewHeight / (2.0f * gridCellSize)) + 1;

    const float gridTop = gridOrigin.y - gridCellSize * gridFactorY;
    const float gridBottom = gridOrigin.y + gridCellSize * gridFactorY;

    sf::VertexArray grid(sf::Lines);

    for (float x = gridLeft; x <= gridRight; x += gridCellSize)
    {
        grid.append(sf::Vertex(sf::Vector2f(x, gridTop), gridColor));
        grid.append(sf::Vertex(sf::Vector2f(x, gridBottom), gridColor));
    }

    for (float y = gridTop; y <= gridBottom; y += gridCellSize)
    {
        grid.append(sf::Vertex(sf::Vector2f(gridLeft, y), gridColor));
        grid.append(sf::Vertex(sf::Vector2f(gridRight, y), gridColor));
    }

    window.draw(grid);
}

void drawWalls(sf::RenderWindow& window, const std::vector<Wall>& walls)
{
    sf::VertexArray lines(sf::Lines);

    for (const Wall& wall : walls)
    {
        lines.append(sf::Vertex(wall.a, sf::Color::White));
        lines.append(sf::Vertex(wall.b, sf::Color::White));
    }

    window.draw(lines);
}
