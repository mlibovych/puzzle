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
        game->m_EventSystem->ProccedEvent();
        game->m_Tetromino = ::std::make_unique<Tetromino> ();
        game->m_TetrominoGhost = ::std::make_unique<Tetromino> (*game->m_Tetromino.get());
        game->ChangeState(States::POSITIONING);
    }
    
    GameState::Tick(context);
}

void SpawnState::Enter() noexcept
{   
    auto game = m_game.lock();

    game->m_FrameTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
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
    if (now - game->m_FrameTime > game->m_FallSpeed) {
        game->m_FrameTime = now;
        game->m_Tetromino->moveDown();
    }

    //side
    if (now - game->m_SideMovingTime > game->m_SideSpeed) {
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

float GetAlpha(float delta) {
    float res = 1.0f - delta * (1.0f / g_LineCompleatedAnimationTime);

    if (res <= 0) {
        return 0;
    }

    return res;
}

void LineCompleatedState::GetColor(::MiniKit::Graphics::Color& color, float delta) noexcept
{
    float res = 1.0f - delta * (1.0f / g_LineCompleatedAnimationTime);

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
        m_Value = 0.0f;
        game->m_GridManager->ClearLines();
        game->ChangeState(States::SPAWN);
    }
    for (int line : game->m_GridManager->GetCompleatedLines()) {
        for (int x = 0; x < g_FieldWidth; x++) {
            if (game->m_Field[line][x]) {
                GetColor(game->m_Field[line][x]->color, delta);
                game->m_Field[line][x]->color.alpha = GetAlpha(m_Value);
            }
        }
    }
    
    GameState::Tick(context);
}

void LineCompleatedState::Enter() noexcept
{   
    m_Value = 0.0f;
}