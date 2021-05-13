#include "StateMachine.h"
#include "Game.h"

using ::MiniKit::Platform::Keycode;

StateMachine::StateMachine(std::shared_ptr<Game> game) : m_game(game)
{

}

StateMachine::~StateMachine()
{

}

GameState::GameState(std::shared_ptr<Game> game) : StateMachine(game)
{

}

GameState::~GameState()
{

}

void GameState::Tick(::MiniKit::Engine::Context& context) noexcept
{   
    auto game = m_game.lock();

    if (game->m_EventSystem->HaveEvents()) {
        game->m_EventSystem->ProccedEvent();
    }
    game->DrawField(context);
}

void GameState::KeyDown(const ::MiniKit::Platform::KeyEvent& event) noexcept
{

}

void GameState::KeyUp(const ::MiniKit::Platform::KeyEvent& event) noexcept
{

}

SpawnState::SpawnState(std::shared_ptr<Game> game) :
            GameState(game)
{
    
}

SpawnState::~SpawnState()
{

}

void SpawnState::Tick(::MiniKit::Engine::Context& context) noexcept
{
    auto game = m_game.lock();

    //events
    if (game->m_EventSystem->HaveEvents() &&
        game->m_EventSystem->GetNextEventType() == EventType::LINES_COMPLEATED_EVENT) {
        game->ChangeState(States::LINE_COMPLEATED);
    }
    else {
        //tereomino
        std::mt19937 gen(m_Random());
        std::uniform_int_distribution<> dis(0, game->m_Tetrominos.size() - 1);
        int position = dis(gen);

        game->m_EventSystem->ProccedEvent();
        game->m_Tetromino = ::std::make_unique<Tetromino> (*game->m_Tetrominos[position].get());
        game->m_Tetromino->m_X = 3;
        game->m_Tetromino->m_Y = 0;
        game->m_TetrominoGhost = ::std::make_unique<Tetromino> (*game->m_Tetromino.get());

        game->ChangeState(States::POSITIONING);
    }
    
    GameState::Tick(context);
}

void SpawnState::Enter() noexcept
{   


}

PositioningState::PositioningState(std::shared_ptr<Game> game) :
            GameState(game)
{
    m_DirectionsQueue[Direction::LEFT] = 0;
    m_DirectionsQueue[Direction::RIGHT] = 0;

    m_DirectionStep[Direction::LEFT] = -1;
    m_DirectionStep[Direction::RIGHT] = 1;
}

PositioningState::~PositioningState()
{

}

void PositioningState::Enter() noexcept
{
    m_DownValue = 0.0f;
    m_SideValue = 0.0f;
    m_LockValue = 0.0f;
    m_Lock = false;
}

void PositioningState::Tick(::MiniKit::Engine::Context& context) noexcept
{   
    
    auto game = m_game.lock();
    
    if (!game) {
        return;
    }
 
    //ghost
    game->GetGhostPosition();
    
    //check state
    if (game->CheckCollision(game->m_Tetromino.get())) {
        if (m_LockValue >= game->m_LockDelay) {
            game->m_GridManager->AddToField();
            game->ChangeState(States::SPAWN);
        }
        else {
            m_LockValue += context.GetFrameDelta();
            if (!m_Lock) {
                m_Lock = true;
                game->m_Tetromino->m_Color.alpha = 0.8f;
            }
        }
    }
    else {
        if (m_Lock) {
            m_Lock = false;
            m_LockValue = 0.0f;
            game->m_Tetromino->m_Color.alpha = 1.0f;
        }
    }

    //moving
    
    //down
    m_DownValue += context.GetFrameDelta();

    auto speed = m_SoftDrop ? game->m_SoftDropSpeed : game->m_FallSpeed;

    if (m_DownValue > speed) {
        m_DownValue = 0.0f;
        if (!m_Lock) {
            game->m_Tetromino->moveDown();
        }
    }

    //side
    m_SideValue += context.GetFrameDelta();

    if (m_SideValue > game->m_SideSpeed) {
        m_SideValue = 0.0f;
        for (const auto& [key, value] : m_DirectionsQueue) {
            if (value == 1) {
                game->MoveSide(m_DirectionStep[key]);
                break;
            }
        }
    }

    GameState::Tick(context);
}

void PositioningState::KeyDown(const ::MiniKit::Platform::KeyEvent& event) noexcept
{
    switch (event.keycode)
    {
        case  Keycode::KeyLeft:
        {
            Start(Direction::LEFT);
            break;
        }
        case  Keycode::KeyRight:
        {   
            Start(Direction::RIGHT);
            break;
        }
        case Keycode::KeyDown:
        {
            m_SoftDrop = true;
            break;
        }
        default:
            break;
    }
}

void PositioningState::KeyUp(const ::MiniKit::Platform::KeyEvent& event) noexcept
{
    switch (event.keycode)
    {
        case  Keycode::KeyLeft:
        {
            Stop(Direction::LEFT);
            break;
        }
        case  Keycode::KeyRight:
        {   
            Stop(Direction::RIGHT);
            break;
        }
        case Keycode::KeyDown:
        {
            m_SoftDrop = false;
            break;
        }
        default:
            break;
    }
}

void PositioningState::Start(Direction direction) noexcept
{
    auto game = m_game.lock();
    game->MoveSide(m_DirectionStep[direction]);
    m_SideValue = 0.0f;
    
    for (auto& [key, value] : m_DirectionsQueue) {
        if (value) {
            value++;
        }
    }
    m_DirectionsQueue[direction] = 1;
}

void PositioningState::Stop(Direction direction) noexcept
{
    for (auto& [key, value] : m_DirectionsQueue) {
        if (value && value > m_DirectionsQueue[direction]) {
            value--;
        }
    }
    m_DirectionsQueue[direction] = 0;
}

LineCompleatedState::LineCompleatedState(std::shared_ptr<Game> game) :
            GameState(game)
{

}

LineCompleatedState::~LineCompleatedState()
{
    
}

float LineCompleatedState::GetAlpha() noexcept
{
    float res = g_LineCompleatedColor.alpha - m_Value * (g_LineCompleatedColor.alpha / g_LineCompleatedAnimationTime);

    if (res <= 0) {
        return 0;
    }

    return res;
}

void LineCompleatedState::GetColor(::MiniKit::Graphics::Color& color, float delta) noexcept
{
    float res = 1.0f - m_Value * (1.0f / g_LineCompleatedAnimationTime);

    if (res <= 0) {
        color = g_LineCompleatedColor;
    }
    else {
        color.red = color.red + ((g_LineCompleatedColor.red - color.red) / (g_LineCompleatedAnimationTime - m_Value)) * (delta);
        color.blue = color.blue + ((g_LineCompleatedColor.blue - color.blue) / (g_LineCompleatedAnimationTime - m_Value)) * (delta);
        color.green = color.green + ((g_LineCompleatedColor.green - color.green) / (g_LineCompleatedAnimationTime - m_Value)) * (delta);
    }
}

void LineCompleatedState::Tick(::MiniKit::Engine::Context& context) noexcept
{   
    
    auto game = m_game.lock();
    
    if (!game) {
        return;
    }

    auto delta = context.GetFrameDelta();
    m_Value += delta;
    if (m_Value >= g_LineCompleatedAnimationTime) {
        game->m_ScoreManager->AddtoScore();
        game->m_GridManager->ClearLines();
        game->ChangeState(States::SPAWN);
    }
    for (int line : game->m_GridManager->GetCompleatedLines()) {
        for (int x = 0; x < g_FieldWidth; x++) {
            if (game->m_Field[line][x]) {
                GetColor(game->m_Field[line][x]->color, delta);
                game->m_Field[line][x]->color.alpha = GetAlpha();
            }
        }
    }
    
    GameState::Tick(context);
}

void LineCompleatedState::Enter() noexcept
{   
    m_Value = 0.0f;
    //recalculate speed
}