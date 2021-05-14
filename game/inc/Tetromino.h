#pragma once

#include <array>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

#include <MiniKit/MiniKit.hpp>

class Game;

using Shape = std::vector<std::vector<int>>;

struct Tetromino {
    int m_X, m_Y;
    ::MiniKit::Graphics::Color m_Color{ 1.0f, 1.0f, 1.0f, 1.0f };
    Shape m_Shape;
    int m_SpawnFrequency { 1 };

    Tetromino();
    Tetromino(const ::MiniKit::Graphics::Color& color,
              Shape& shape,
              int frequency);
    Tetromino(const Tetromino& other);
    ~Tetromino();

    void MoveDown() noexcept;
    void MoveSide(int step) noexcept;
    void RotateRight() noexcept;
    void RotateLeft() noexcept;
};
