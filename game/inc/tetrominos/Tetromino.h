#pragma once

#include <array>
#include <iostream>
#include <string>
#include <unordered_map>

#include <MiniKit/MiniKit.hpp>

class Game;

class Tetromino {
    friend class Game;

    int m_X, m_Y;
    ::MiniKit::Graphics::Color m_Color{ 0.3f, 1.0f, 0.61f, 1.0f };
    std::array<std::array<int, 3>, 2> m_Shape;
public:
    Tetromino();
    Tetromino(const Tetromino& other);
    ~Tetromino();

    void moveDown() noexcept;
    void moveSide(int step) noexcept;
};
