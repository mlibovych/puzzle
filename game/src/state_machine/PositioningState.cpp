#include <PositioningState.h>
#include <Game.h>

using ::MiniKit::Platform::Keycode;

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