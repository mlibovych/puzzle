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

void Tetromino::MoveDown() noexcept
{
    m_Y++;
}

void Tetromino::MoveSide(int step) noexcept
{
    m_X += step;
}

void Tetromino::RotateRight() noexcept
{
    for (int y = 0; y < m_Shape.size() / 2; y++) {
        for (int x = y; x < m_Shape.size() - y - 1; x++) {
            int temp = m_Shape[y][x];

            m_Shape[y][x] = m_Shape[m_Shape.size() - 1 - x][y];

            m_Shape[m_Shape.size() - 1 - x][y] = m_Shape[m_Shape.size() - 1 - y][m_Shape.size() - 1 - x];

            m_Shape[m_Shape.size() - 1 - y][m_Shape.size() - 1 - x] = m_Shape[x][m_Shape.size() - 1 - y];

            m_Shape[x][m_Shape.size() - 1 - y] = temp;
        }
    }
}

void Tetromino::RotateLeft() noexcept
{
    for (int y = 0; y < m_Shape.size() / 2; y++) {
        for (int x = y; x < m_Shape.size() - y - 1; x++) {
            int temp = m_Shape[y][x];

            m_Shape[y][x] = m_Shape[x][m_Shape.size() - 1 - y];

            m_Shape[x][m_Shape.size() - 1 - y] = m_Shape[m_Shape.size() - 1 - y][m_Shape.size() - 1 - x];

            m_Shape[m_Shape.size() - 1 - y][m_Shape.size() - 1 - x] = m_Shape[m_Shape.size() - 1 - x][y];

            m_Shape[m_Shape.size() - 1 - x][y] = temp;
        }
    }
}