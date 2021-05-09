#include <Tetromino.h>

Tetromino::Tetromino() 
{   
    for (auto& row : m_Shape) {
        for (auto& col : row) {
            col = 0;
        }
    }
    //init
    m_Shape[0][1] = 1;
    m_Shape[1][0] = 1;
    m_Shape[1][1] = 1;
    m_Shape[1][2] = 1;
    //

    //position
    m_X = 3;
    m_Y = 0;
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