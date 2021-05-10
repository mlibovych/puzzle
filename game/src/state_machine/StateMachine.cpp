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
    game->m_FrameTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
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
        game->m_EventSystem->ProccedEvent();
        game->m_Tetromino = ::std::make_unique<Tetromino> ();
        game->m_TetrominoGhost = ::std::make_unique<Tetromino> (*game->m_Tetromino.get());
        game->ChangeState(States::POSITIONING);
    }
    
    GameState::Tick(context);
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

void PositioningState::Tick(::MiniKit::Engine::Context& context) noexcept
{   
    
    auto game = m_game.lock();
    
    if (!game) {
        return;
    }

    //moving
    uint64_t now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    
    //down
    if (now - game->m_FrameTime > 200) {
        game->m_FrameTime = now;
        game->m_Tetromino->moveDown();
    }

    //side
    if (now - game->m_SideMovingTime > g_SideMovingSpeed) {
        game->m_SideMovingTime = now;
        for (const auto& [key, value] : m_DirectionsQueue) {
            if (value == 1) {
                game->MoveSide(m_DirectionStep[key]);
            }
        }
    }
 
    //ghost
    game->GetGhostPosition();
    
    //check state
    try {
        game->CheckCollision(game->m_Tetromino.get());
    }
    catch (bool) {
        game->m_GridManager->AddToField();
        game->ChangeState(States::SPAWN);
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
        default:
            break;
    }
}

void PositioningState::Start(Direction direction) noexcept
{
    auto game = m_game.lock();
    game->MoveSide(m_DirectionStep[direction]);
    game->m_SideMovingTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

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

void LineCompleatedState::Tick(::MiniKit::Engine::Context& context) noexcept
{   
    
    auto game = m_game.lock();
    
    if (!game) {
        return;
    }

    game->m_GridManager->ClearLines();
    game->ChangeState(States::SPAWN);
    
    GameState::Tick(context);
}