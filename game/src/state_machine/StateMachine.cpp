#include "StateMachine.h"
#include "Game.h"

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

SpawnState::SpawnState(std::shared_ptr<Game> game) :
            GameState(game)
{
    
}

SpawnState::~SpawnState()
{

}

void SpawnState::Tick(::MiniKit::Engine::Context& context, ::MiniKit::Graphics::DrawInfo& drawSurface, ::MiniKit::Graphics::CommandBuffer& commandBuffer) noexcept
{
    auto game = m_Game.lock();

    //events
    if (game->m_EventSystem->HaveEvents() &&
        game->m_EventSystem->GetNextEventType() == EventType::LINES_COMPLETED_EVENT) {
        game->ChangeState(States::LINE_COMPLEATED);
    }
    else {
        //tereomino
        std::mt19937 gen(m_Random());
        std::uniform_int_distribution<> dis(0, game->m_TetrominosFrequency - 1);
        int position = dis(gen);
        int sumFrequency = 0;

        game->m_EventSystem->ProccedEvent();
        game->m_Tetromino = ::std::make_unique<Tetromino> (*game->m_NextTetromino);

        for (auto& tetromino : game->m_Tetrominos) {
            sumFrequency +=tetromino->m_SpawnFrequency;
            if (sumFrequency >= position) {
                game->m_NextTetromino = ::std::make_unique<Tetromino> (*tetromino.get());
                break;
            }
        }
        
        int displacement = game->m_Tetromino->m_Shape.size() % 2 ? 1 : 0;

        game->m_Tetromino->m_X = g_FieldWidth / 2 - game->m_Tetromino->m_Shape.size() / 2 - displacement;
        game->m_Tetromino->m_Y = 0 - game->m_Tetromino->m_Shape.size() / 2;
        game->m_TetrominoGhost = ::std::make_unique<Tetromino> (*game->m_Tetromino.get());

        game->ChangeState(States::POSITIONING);

        for (size_t y = 0; y < game->m_Tetromino->m_Shape.size(); y++) {
            for (size_t x = 0; x < game->m_Tetromino->m_Shape[y].size(); x++) {
                if (game->m_Tetromino->m_Shape[y][x]) {
                    if (game->m_Tetromino->m_Y + static_cast<int> (y) >= 0 &&
                        game->m_Blocks[game->m_Tetromino->m_Y + y][game->m_Tetromino->m_X + x]) {
                        game->ChangeState(States::NEW_GAME);
                        break;
                    }
                }
            }
        }
    }
    GameState::Tick(context, drawSurface, commandBuffer);
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

void PositioningState::Tick(::MiniKit::Engine::Context& context, ::MiniKit::Graphics::DrawInfo& drawSurface, ::MiniKit::Graphics::CommandBuffer& commandBuffer) noexcept
{   
    auto game = m_Game.lock();
    
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
    
    if (game->m_Tetromino) {
    //down
        m_DownValue += context.GetFrameDelta();

        if (m_DownValue > game->m_FallSpeed) {
            m_DownValue = 0.0f;
            if (!m_Lock) {
                game->m_Tetromino->MoveDown();
                if (m_SoftDrop) {
                    game->m_ScoreManager->AddtoScore(1);
                }
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
    }
    GameState::Tick(context, drawSurface, commandBuffer);

}

void PositioningState::KeyDown(const ::MiniKit::Platform::KeyEvent& event) noexcept
{
    switch (event.keycode)
    {
        case Keycode::KeyLeft:
        {
            Start(Direction::LEFT);
            break;
        }
        case Keycode::KeyRight:
        {   
            Start(Direction::RIGHT);
            break;
        }
        case Keycode::KeyDown:
        {   
            SoftDrop(true);
            break;
        }
        case Keycode::KeyUp:
        {
            RotateRight();
            break;
        }
        case Keycode::KeyZ:
        {
            RotateLeft();
            break;
        }
        case Keycode::KeySpace:
        {   
            HardDrop();
            break;
        }
        case Keycode::KeyC:
        {
            auto game = m_Game.lock();

            game->ChangeState(States::PAUSE);
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
        case Keycode::KeyLeft:
        {
            Stop(Direction::LEFT);
            break;
        }
        case Keycode::KeyRight:
        {   
            Stop(Direction::RIGHT);
            break;
        }
        case Keycode::KeyDown:
        {
            SoftDrop(false);
            break;
        }
        default:
            break;
    }
}

void PositioningState::SoftDrop(bool drop) noexcept
{
    auto game = m_Game.lock();
    m_SoftDrop = drop;

    if (drop) {
        game->m_FallSpeed /= 6;
    }
    else {
        game->m_FallSpeed *= 6;
    }   
}

void PositioningState::HardDrop() noexcept
{
    auto game = m_Game.lock();

    m_LockValue = game->m_LockDelay;
    game->m_ScoreManager->AddtoScore(g_FieldHeight - game->m_Tetromino->m_Y);
    game->m_Tetromino->m_Y = game->m_TetrominoGhost->m_Y;
}

void PositioningState::RotateRight() noexcept 
{
    auto game = m_Game.lock();
    
    game->m_Tetromino->RotateRight();
    game->m_TetrominoGhost->RotateRight();

    if (game->CheckSideCollision(0)) {
        if (!game->CheckSideCollision(m_DirectionStep[Direction::RIGHT])) {
            game->MoveSide(m_DirectionStep[Direction::RIGHT]);
        }
        else if (!game->CheckSideCollision(m_DirectionStep[Direction::LEFT])) {
            game->MoveSide(m_DirectionStep[Direction::LEFT]);
        }
        else {
            game->m_Tetromino->m_Y--;
            if (game->CheckSideCollision(0)) {
                game->m_Tetromino->m_Y++;
                game->m_Tetromino->RotateLeft();
                game->m_TetrominoGhost->RotateLeft();
            }
        }
    }
}

void PositioningState::RotateLeft() noexcept 
{
    auto game = m_Game.lock();

    game->m_Tetromino->RotateLeft();
    game->m_TetrominoGhost->RotateLeft();

    if (game->CheckSideCollision(0)) {
        if (!game->CheckSideCollision(m_DirectionStep[Direction::RIGHT])) {
            game->MoveSide(m_DirectionStep[Direction::RIGHT]);
        }
        else if (!game->CheckSideCollision(m_DirectionStep[Direction::LEFT])) {
            game->MoveSide(m_DirectionStep[Direction::LEFT]);
        }
        else {
            game->m_Tetromino->m_Y--;
            if (game->CheckSideCollision(0)) {
                game->m_Tetromino->m_Y++;
                game->m_Tetromino->RotateRight();
                game->m_TetrominoGhost->RotateRight();
            }
        }
    }
}

void PositioningState::Start(Direction direction) noexcept
{
    auto game = m_Game.lock();
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

void LineCompleatedState::Tick(::MiniKit::Engine::Context& context, ::MiniKit::Graphics::DrawInfo& drawSurface, ::MiniKit::Graphics::CommandBuffer& commandBuffer) noexcept
{   
    auto game = m_Game.lock();
    
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
            if (game->m_Blocks[line][x]) {
                GetColor(game->m_Blocks[line][x]->color, delta);
                game->m_Blocks[line][x]->color.alpha = GetAlpha();
            }
        }
    }
    
    GameState::Tick(context, drawSurface, commandBuffer);
}

void LineCompleatedState::Enter() noexcept
{   
    m_Value = 0.0f;
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

    m_Value = 0.0f;

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
    game->m_Pause = false;

    if (game->m_Settings->IsOld()) {
        game->UpdateSettings();
    }
}

PauseState::PauseState(std::shared_ptr<Game> game) :
            GameState(game)
{
    
}

PauseState::~PauseState()
{

}

void PauseState::KeyDown(const ::MiniKit::Platform::KeyEvent& event) noexcept
{
    switch (event.keycode)
    {
        case Keycode::KeyC:
        {
            auto game = m_Game.lock();
    
            game->ChangeState(States::POSITIONING);
            break;
        }
        default:
            break;
    }
}

void PauseState::Tick(::MiniKit::Engine::Context& context, ::MiniKit::Graphics::DrawInfo& drawSurface, ::MiniKit::Graphics::CommandBuffer& commandBuffer) noexcept
{
   auto game = m_Game.lock();

    // game->m_Menu->Draw(context);
}

void PauseState::Enter() noexcept
{
    auto game = m_Game.lock();

    game->m_Pause = true;
}

void PauseState::Exit() noexcept
{
    auto game = m_Game.lock();

    if (game->m_Settings->IsOld()) {
        game->UpdateSettings();
    }
    game->m_Pause = false;
}
