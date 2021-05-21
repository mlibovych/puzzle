#include <StateMachine.h>
#include <Game.h>

using ::MiniKit::Platform::Keycode;

StateMachine::StateMachine(std::shared_ptr<Game> game) : m_Game(game)
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

void GameState::Tick(::MiniKit::Engine::Context& context, ::MiniKit::Graphics::DrawInfo& drawSurface, ::MiniKit::Graphics::CommandBuffer& commandBuffer) noexcept
{   
    auto game = m_Game.lock();

    if (game->m_EventSystem->HaveEvents()) {
        game->m_EventSystem->ProccedEvent();
    }
    game->Draw(context, drawSurface, commandBuffer);
}

void GameState::KeyDown(const ::MiniKit::Platform::KeyEvent& event) noexcept
{

}

void GameState::KeyUp(const ::MiniKit::Platform::KeyEvent& event) noexcept
{

}

NewGameState::NewGameState(std::shared_ptr<Game> game) :
            GameState(game)
{

}

NewGameState::~NewGameState()
{
    
}

void NewGameState::Tick(::MiniKit::Engine::Context& context, ::MiniKit::Graphics::DrawInfo& drawSurface, ::MiniKit::Graphics::CommandBuffer& commandBuffer) noexcept
{   
    
    auto game = m_Game.lock();
    
    if (!game) {
        return;
    }
    
    game->ChangeState(States::SPAWN);
    
    GameState::Tick(context, drawSurface, commandBuffer);
}

void NewGameState::Enter() noexcept
{   
    auto game = m_Game.lock();
    
    if (!game) {
        return;
    }

    //next tetromino
    std::mt19937 gen(m_Random());
    std::uniform_int_distribution<> dis(0, game->m_TetrominosFrequency - 1);
    int position = dis(gen);
    int sumFrequency = 0;

    for (auto& tetromino : game->m_Tetrominos) {
        sumFrequency +=tetromino->m_SpawnFrequency;
        if (sumFrequency >= position) {
            game->m_NextTetromino = ::std::make_unique<Tetromino> (*tetromino.get());
            break;
        }
    }

    //clear old game
    game->m_GridManager->ClearField();
    game->m_ScoreManager->ClearScore();
    game->m_FallSpeed = g_FallSpeed;

    if (game->m_Settings->IsOld()) {
        game->UpdateSettings();
    }

    game->m_Pause = false;
}
