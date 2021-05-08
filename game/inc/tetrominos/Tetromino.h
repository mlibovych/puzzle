#pragma once

#include <array>
#include <iostream>
#include <string>

#include <MiniKit/MiniKit.hpp>

class Tetromino {
public:
    Tetromino();
    ~Tetromino();

    void moveDown() noexcept;

    int m_X, m_Y;
    std::array<std::array<int, 4>, 4> m_Shape;
};
