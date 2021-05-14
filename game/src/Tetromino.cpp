#include <Tetromino.h>

Tetromino::Tetromino() 
{   
    
}

Tetromino::Tetromino(const ::MiniKit::Graphics::Color& color,
                    std::vector<std::vector<int>>& shape,
                    int frequency) 
                    : m_Color(color), m_Shape(shape), m_SpawnFrequency(frequency)
{
    
}
    

Tetromino::Tetromino(const Tetromino& other) {
    m_Shape = other.m_Shape;
    m_X = other.m_X;
    m_Y = other.m_Y;
    m_Color = other.m_Color;
}

Tetromino::~Tetromino()
{

}

void Tetromino::moveDown() noexcept
{
    m_Y++;
}

void Tetromino::moveSide(int step) noexcept
{
    m_X += step;
}