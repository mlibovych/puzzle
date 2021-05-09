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
    ::MiniKit::Graphics::Color m_Color{ 0.3f, 1.0f, 0.61f, 1.0f };
    std::array<std::array<int, 4>, 4> m_Shape;

};
