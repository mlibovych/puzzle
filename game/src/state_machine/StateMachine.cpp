#include "StateMachine.h"
#include "Game.h"

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
    m_game.lock()->DrawField(context);
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

    game->m_Tetromino = ::std::make_unique<Tetromino> ();
    game->ChangeState(States::POSITIONING);
    GameState::Tick(context);
}

PositioningState::PositioningState(std::shared_ptr<Game> game) :
            GameState(game)
{

}

PositioningState::~PositioningState()
{

}

void PositioningState::Tick(::MiniKit::Engine::Context& context) noexcept
{
    //moving
    auto game = m_game.lock();
    uint64_t now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    
    if (now - game->m_FrameTime > 200) {
        game->m_FrameTime = now;
        game->m_Tetromino->moveDown();
    }
    
    //check state
    try {
        game->CheckCollision();
    }
    catch (bool) {
        game->AddToField();
        game->ChangeState(States::SPAWN);
    }
    GameState::Tick(context);
}
