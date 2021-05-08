#pragma once

#include <array>
#include <iostream>
#include <string>

#include <MiniKit/MiniKit.hpp>

class Game;

class Tetromino {
    friend class Game;
    std::array<std::array<int, 4>, 4> m_Shape;
    float m_X, m_Y;
public:
    Tetromino();
    ~Tetromino();
};
