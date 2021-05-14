#pragma once

#include <array>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

#include <MiniKit/MiniKit.hpp>

class Game;

struct Tetromino {
    int m_X, m_Y;
    ::MiniKit::Graphics::Color m_Color{ 1.0f, 1.0f, 1.0f, 1.0f };
    std::vector<std::vector<int>> m_Shape;
    int m_SpawnFrequency { 1 };

    Tetromino();
    Tetromino(const ::MiniKit::Graphics::Color& color,
              std::vector<std::vector<int>>& shape,
              int frequency);
    Tetromino(const Tetromino& other);
    ~Tetromino();

    void moveDown() noexcept;
    void moveSide(int step) noexcept;
};
