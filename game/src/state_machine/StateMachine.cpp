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
    GameState::Tick(context);
}
