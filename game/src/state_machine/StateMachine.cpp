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

        States state = States::POSITIONING;

        try {
            for (size_t y = 0; y < game->m_Tetromino->m_Shape.size(); y++) {
                for (size_t x = 0; x < game->m_Tetromino->m_Shape[y].size(); x++) {
                    if (game->m_Tetromino->m_Shape[y][x]) {
                        if (game->m_Tetromino->m_Y + static_cast<int> (y) >= 0 &&
                            game->m_Blocks[game->m_Tetromino->m_Y + y][game->m_Tetromino->m_X + x]) {
                            throw(true);
                        }
                    }
                }
            }
        }
        catch (bool) {
            state = States::GAME_OVER;
            game->m_Tetromino = nullptr;
        }
        game->ChangeState(state);
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
   auto app = game->m_App.lock();

   GameState::Tick(context, drawSurface, commandBuffer);
   float x = 0 - 5 * 80 / 2;
   float y = 200;
   app->DrawText(drawSurface, commandBuffer, g_OrangeColor, "Pause", x, y, 80, 90);
}

void PauseState::Enter() noexcept
{
    auto game = m_Game.lock();
    
    game->m_Pause = true;
    game->m_App.lock()->ChangeElement(Element::MENU);
}

void PauseState::Exit() noexcept
{
    auto game = m_Game.lock();

    game->m_Pause = false;
    if (game->m_Settings->IsOld()) {
        game->UpdateSettings();
    }
}

GameOverState::GameOverState(std::shared_ptr<Game> game) : GameState(game), ElementWithButtons(game->m_App.lock())
{
    m_Elements.push_back(Button{"Main menu", true, [&]() {
        auto game = m_Game.lock();

        game->ChangeState(States::NEW_GAME, true);
    }});
    m_Elements.push_back(Button{"New game", true, [&]() {
        auto game = m_Game.lock();

        game->ChangeState(States::NEW_GAME);
    }});
}

GameOverState::~GameOverState()
{

}

void GameOverState::Tick(::MiniKit::Engine::Context& context, ::MiniKit::Graphics::DrawInfo& drawSurface, ::MiniKit::Graphics::CommandBuffer& commandBuffer) noexcept
{
    auto game = m_Game.lock();
    auto app = game->m_App.lock();

    float startX = 0;
    float startY = 1800.0f / 2 - g_Padding * 2 - 400;

    game->Draw(context, drawSurface, commandBuffer);
    if (m_Save) {
        auto& image = app->GetImage("border");

        commandBuffer.SetImage(image);
        drawSurface.tint = g_OrangeColor;
        drawSurface.position = { startX, startY };
        drawSurface.scale = { (m_Invitation.size() * 50.0f + g_Padding * 2) / image.GetSize().width, 300.0f / image.GetSize().height };

        commandBuffer.Draw(drawSurface);

        startX = 0 - (m_Invitation.size() - 1) * 50.0f / 2;
        startY += 60;
        app->DrawText(drawSurface, commandBuffer, g_BlackColor, m_Invitation, startX, startY, 50, 60);
        startY -= 60 * 2;
        startX = 0 - (m_Name.size() - 1) * 40.0f / 2;
        app->DrawText(drawSurface, commandBuffer, g_WhiteColor, m_Name, startX, startY, 40, 50);
        startY -= 50;
    }
    else {
        DrawElementsVertical(drawSurface, commandBuffer,startX, startY, 400, 100);
    }
}

void GameOverState::Enter() noexcept
{
    auto game = m_Game.lock();
    auto app = game->m_App.lock();

    m_Name.clear();
    m_Save = game->m_Save;
}

void GameOverState::KeyDown(const ::MiniKit::Platform::KeyEvent& event) noexcept
{   
    using ::MiniKit::Platform::Keycode;

    if (m_Save) {
        switch (event.keycode)
        {   
            case Keycode::KeyA:
            case Keycode::KeyB:
            case Keycode::KeyC:
            case Keycode::KeyD:
            case Keycode::KeyE:
            case Keycode::KeyF:
            case Keycode::KeyG:
            case Keycode::KeyH:
            case Keycode::KeyI:
            case Keycode::KeyJ:
            case Keycode::KeyK:
            case Keycode::KeyL:
            case Keycode::KeyM:
            case Keycode::KeyN:
            case Keycode::KeyO:
            case Keycode::KeyP:
            case Keycode::KeyQ:
            case Keycode::KeyR:
            case Keycode::KeyS:
            case Keycode::KeyT:
            case Keycode::KeyU:
            case Keycode::KeyV:
            case Keycode::KeyW:
            case Keycode::KeyX:
            case Keycode::KeyY:
            case Keycode::KeyZ:
            {
                m_Name.push_back(static_cast<int> (event.keycode) + 65);
                break;
            }
            case Keycode::KeyBackspace:
            {
                m_Name.pop_back();
                break;
            }
            case Keycode::KeySpace:
            {
                m_Name.push_back(' ');
                break;
            }
            case Keycode::KeyEnter:
            {
                if (!m_Name.empty()) {
                    auto game = m_Game.lock();
                    auto app = game->m_App.lock();

                    app->SaveResult(game->m_Score, m_Name);
                    m_Save = false;
                }
            }
            default:
                break;
        }
    }
    else {
        ElementWithButtons::KeyDown(event);
    }
}
