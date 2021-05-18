#include <Game.h>
#include <imgui.h>

::std::error_code Game::Start(::MiniKit::Engine::Context& context) noexcept
{   
    //settings
    m_Settings = std::make_unique<Settings> ();
    UpdateSettings();

    //event system
    m_EventSystem = std::make_unique<EventSystem>();
    m_GridResolver = std::make_unique<GridResolver> (shared_from_this());
    m_GridManager = std::make_unique<GridManager> (shared_from_this());
    m_ScoreManager = std::make_unique<ScoreManager> (shared_from_this());

    m_EventSystem->Subscribe(EventType::BLOCK_SET_EVENT, m_GridResolver.get());
    m_EventSystem->Subscribe(EventType::LINES_COMPLETED_EVENT, m_GridManager.get());
    m_EventSystem->Subscribe(EventType::LINES_COMPLETED_EVENT, m_ScoreManager.get());

    //states
    m_States[States::SPAWN] = std::make_unique<SpawnState> (shared_from_this());
    m_States[States::POSITIONING] = std::make_unique<PositioningState> (shared_from_this());
    m_States[States::LINE_COMPLEATED] = std::make_unique<LineCompleatedState> (shared_from_this());
    m_States[States::NEW_GAME] = std::make_unique<NewGameState> (shared_from_this());
    m_States[States::PAUSE] = std::make_unique<PauseState> (shared_from_this());

    ChangeState(States::NEW_GAME);

    //window
    auto& window = context.GetWindow();
    window.AddResponder(*this);

    //playing elements
    //images
    auto& graphicsDevice = context.GetGraphicsDevice();

    m_Images["block"] = graphicsDevice.CreateImage(g_BlockPath);
    m_Images["field"] = graphicsDevice.CreateImage(g_FieldPath);
    m_Images["back"] = graphicsDevice.CreateImage(g_BackPath);
    m_Images["border"] = graphicsDevice.CreateImage(g_BorderPath);
    for (int i = 0; i < 10; i++) {
        m_Images[std::to_string(i)] = graphicsDevice.CreateImage("assets/" + std::to_string(i) + ".png");
    }
    for (int i = 65; i <= 90; i++) {
        std::string letter {static_cast<char> (i)};

        m_Images[letter] = graphicsDevice.CreateImage("assets/" + letter + ".png");
    }
    m_Images[" "] = graphicsDevice.CreateImage(g_BlankPath);

    //field background
    const auto& imageSize = m_Images["field"]->GetSize();

    const auto drawableWidth = static_cast<float>(context.GetWindow().GetDrawableWidth());
    const auto drawableHeight = static_cast<float>(context.GetWindow().GetDrawableHeight());
    const auto imagesGridPixelsWidth = g_FieldWidth * g_BlockWidth;
    const auto imagesGridPixelsHeight = g_FieldHeight * imageSize.width;
    const auto imageScaleY = static_cast<float>(drawableHeight - g_Padding * 2) / static_cast<float>(imagesGridPixelsHeight);
    m_BlockScale = {imageScaleY, imageScaleY};

    auto anchorPositionX =  -0.5f * drawableWidth + imageScaleY * imageSize.width / 2 + g_Padding;
    auto anchorPositionY =  0.5f * drawableHeight - imageScaleY * imageSize.width / 2 - g_Padding;

    for (int y = g_FieldHeight - 1; y >= 0; y--) {
        for (int x = 0; x < g_FieldWidth; x++) {
            auto& cell = m_Field[y][x];

            cell.position.x = anchorPositionX + imageScaleY * imageSize.width * x;
            cell.position.y = anchorPositionY - imageScaleY * imageSize.width * y;
        }
    }

    //next tetromino position
    m_NextTetrominoX = ((0.5f * context.GetWindow().GetDrawableWidth() - g_Padding) + (-0.5f * context.GetWindow().GetDrawableWidth() + m_BlockScale.x * m_Images["field"]->GetSize().width * g_FieldWidth + 2 * g_Padding)) / 2 - (3 * g_BlockWidth) / 2;

    //background
    const auto& backSize = m_Images["back"]->GetSize();
    const auto backImageScaleY = static_cast<float>(drawableHeight) / static_cast<float>(backSize.height);
    const auto backImageScaleX = static_cast<float>(drawableWidth) / static_cast<float>(backSize.width);
    m_BackgroundScale = {backImageScaleX, backImageScaleY};
    
    //numbers
    auto logoHeight = 60.0f + 20.f;
    m_LogoY = 0.5f * context.GetWindow().GetDrawableHeight() - g_Padding - 0.5f * logoHeight;
    m_NextTetrominoTitleY = m_LogoY - (g_Padding * 3 + 0.5f * logoHeight + 0.5f * 60);
    m_NextTetrominoY = m_NextTetrominoTitleY - (3 * g_BlockWidth) / 2;

    m_ScoreTitleY = m_NextTetrominoY - (g_Padding * 3 + (4 * g_BlockWidth) / 2 + 0.5f * 60);;
    m_ScoreNumberY = m_ScoreTitleY - 0.5f * 60 * 2 - 3 * g_Padding;
    m_LevelTitleY = m_ScoreNumberY - 0.5f * 60 * 2 - 3 * g_Padding;
    m_LevelNumberY = m_LevelTitleY - 0.5f * 60 * 2 - 3 * g_Padding;
    m_LinesTitleY = m_LevelNumberY - 0.5f * 60 * 2 - 3 * g_Padding;
    m_LinesNumberY = m_LinesTitleY - 0.5f * 60 * 2 - 3 * g_Padding;
    return {};
}

::std::error_code Game::Shutdown(::MiniKit::Engine::Context& context) noexcept
{
    return {};
}

void Game::Tick(::MiniKit::Engine::Context& context) noexcept
{   
    m_States[m_State]->Tick(context);
}

void Game::UpdateSettings()
{   
    m_Tetrominos.clear();
    m_Settings->Update();
    
    decltype(auto) ghost = m_Settings->GetData().GetInt("ghoste");
    m_Ghost = ghost;

    m_TetrominosFrequency = 0;

    decltype(auto) tetrominos = m_Settings->GetData().GetVector("tetrominos");

    for (size_t i = 0; i < tetrominos.size(); i++) {
        FormatObject tetromino = std::get<FormatObject> (tetrominos[i]);

        //frequency
        int tetrominoFrequency = tetromino.GetInt("frequency");
        m_TetrominosFrequency += tetrominoFrequency;

        //shape
        FormatObject shape = tetromino.GetObject("shape");
        int rows = shape.GetInt("rows");

        std::vector<std::vector<int>> tetrominoShape;
        tetrominoShape.reserve(rows);

        for (int i = 0; i < rows; i++) {
            std::vector<int> tetrominoRow;
            decltype(auto) row = shape.GetVector(std::to_string(i));
            
            tetrominoRow.reserve(rows);
            for (auto el : row) {
                tetrominoRow.push_back(std::get<int> (el));
            }
            tetrominoShape.push_back(tetrominoRow);
        }

        //color
        ::MiniKit::Graphics::Color tetrominoColor;
        decltype(auto) color = tetromino.GetVector("color");
        tetrominoColor.red = std::get<float> (color[0]);
        tetrominoColor.green = std::get<float> (color[1]);
        tetrominoColor.blue = std::get<float> (color[2]);
        tetrominoColor.alpha = std::get<float> (color[3]);

        m_Tetrominos.push_back(std::make_unique<Tetromino> (tetrominoColor, tetrominoShape, tetrominoFrequency));
    }
}

void Game::DrawField(::MiniKit::Engine::Context& context, ::MiniKit::Graphics::DrawInfo& drawSurface,
                          ::MiniKit::Graphics::CommandBuffer& commandBuffer) noexcept
{
    //draw field
    commandBuffer.SetImage(*m_Images["field"]);

    for (const auto& row : m_Field) {
        for (const auto& col : row) {
            drawSurface.tint = col.color;
            drawSurface.position = col.position;
            drawSurface.scale = m_BlockScale;

            commandBuffer.Draw(drawSurface);
        }
    }
}

void Game::DrawBlocks(::MiniKit::Engine::Context& context, ::MiniKit::Graphics::DrawInfo& drawSurface,
                      ::MiniKit::Graphics::CommandBuffer& commandBuffer) noexcept
{
    commandBuffer.SetImage(*m_Images["block"]);

    //ghost
    if (m_Ghost && m_TetrominoGhost) {
        for (size_t y = 0; y < m_TetrominoGhost->m_Shape.size(); y++) {
            for (size_t x = 0; x < m_TetrominoGhost->m_Shape[y].size(); x++) {
                if (m_TetrominoGhost->m_Shape[y][x] && m_TetrominoGhost->m_Y + static_cast<int> (y) >= 0) {
                    drawSurface.tint = m_Tetromino->m_Color;
                    drawSurface.tint.alpha = 0.5;
                    drawSurface.position.x = m_Field[m_TetrominoGhost->m_Y + y][m_TetrominoGhost->m_X + x].position.x;
                    drawSurface.position.y = m_Field[m_TetrominoGhost->m_Y + y][m_TetrominoGhost->m_X + x].position.y;
                    drawSurface.scale = m_BlockScale;

                    commandBuffer.Draw(drawSurface);
                }
            }
        }
    }

    //tetromino
    if (m_Tetromino) {
        for (size_t y = 0; y < m_Tetromino->m_Shape.size(); y++) {
            for (size_t x = 0; x < m_Tetromino->m_Shape[y].size(); x++) {
                if (m_Tetromino->m_Shape[y][x] && m_Tetromino->m_Y + static_cast<int> (y) >= 0) {
                    drawSurface.tint = m_Tetromino->m_Color;
                    drawSurface.position.x = m_Field[m_Tetromino->m_Y + y][m_Tetromino->m_X + x].position.x;
                    drawSurface.position.y = m_Field[m_Tetromino->m_Y + y][m_Tetromino->m_X + x].position.y;
                    drawSurface.scale = m_BlockScale;

                    commandBuffer.Draw(drawSurface);
                }
            }
        }
    }

    //next
    const auto& imageSize = m_Images["field"]->GetSize();
    auto displace = m_NextTetromino->m_Shape.size() == 2 ? 1 : 0;

    if (m_NextTetromino) {
        for (size_t y = 0; y < m_NextTetromino->m_Shape.size(); y++) {
            for (size_t x = 0; x < m_NextTetromino->m_Shape[y].size(); x++) {
                if (m_NextTetromino->m_Shape[y][x]) {
                    drawSurface.tint = m_NextTetromino->m_Color;
                    drawSurface.position.x = m_BlockScale.x * imageSize.width * ( x + displace) + m_NextTetrominoX;
                    drawSurface.position.y = m_NextTetrominoY - m_BlockScale.y * imageSize.height * y - g_Padding;
                    drawSurface.scale = m_BlockScale;

                    commandBuffer.Draw(drawSurface);
                }
            }
        }
    }

    // draw blocks
    for (size_t y = 0; y < m_Blocks.size(); y++) {
        for (size_t x = 0; x < m_Blocks[y].size(); x++) {
            if (m_Blocks[y][x]) {
                drawSurface.tint = m_Blocks[y][x]->color;
                drawSurface.position = m_Field[y][x].position;
                drawSurface.scale = m_BlockScale;

                commandBuffer.Draw(drawSurface);
            }
        }
    }
}

void Game::GetGhostPosition()
{
    for (int y = m_Tetromino->m_Y; y < g_FieldHeight; y++) {
        m_TetrominoGhost->m_Y = y;
        if (CheckCollision(m_TetrominoGhost.get())) {
            break;
        }
    }
    m_TetrominoGhost->m_X = m_Tetromino->m_X;
}

void Game::DrawBackground(::MiniKit::Engine::Context& context, ::MiniKit::Graphics::DrawInfo& drawSurface,
                      ::MiniKit::Graphics::CommandBuffer& commandBuffer) noexcept
{
    commandBuffer.SetImage(*m_Images["back"]);

    drawSurface.tint = g_BackgroundColor;
    drawSurface.position.x = 0;
    drawSurface.position.y = 0;
    drawSurface.scale = m_BackgroundScale;

    commandBuffer.Draw(drawSurface);
}

void Game::DrawNumber(::MiniKit::Engine::Context& context, ::MiniKit::Graphics::DrawInfo& drawSurface,
                      ::MiniKit::Graphics::CommandBuffer& commandBuffer, int number) noexcept
{
    auto imageX = 0.5f * static_cast<float>(context.GetWindow().GetDrawableWidth() - g_Padding);
    do {
        auto digit = number % 10;

        number /= 10;

        const auto& imageSize = m_Images[std::to_string(digit)]->GetSize();
        const auto imageScaleX = 50 / static_cast<float> (imageSize.width);
        const auto imageScaleY = 60 / static_cast<float> (imageSize.height);

        drawSurface.position.x = imageX - imageSize.width * imageScaleX;
        drawSurface.tint = g_OrangeColor;
        drawSurface.scale = {imageScaleX, imageScaleY};

        commandBuffer.SetImage(*m_Images[std::to_string(digit)]);
        commandBuffer.Draw(drawSurface);
        
        imageX -= imageSize.width * imageScaleX;
    }
    while (number);
}

void Game::DrawScore(::MiniKit::Engine::Context& context, ::MiniKit::Graphics::DrawInfo& drawSurface,
                      ::MiniKit::Graphics::CommandBuffer& commandBuffer) noexcept
{   
    drawSurface.position.y = m_ScoreNumberY;
    DrawNumber(context, drawSurface, commandBuffer, m_Score);
    drawSurface.position.y = m_LevelNumberY;
    DrawNumber(context, drawSurface, commandBuffer, m_Level);
    drawSurface.position.y = m_LinesNumberY;
    DrawNumber(context, drawSurface, commandBuffer, m_ClearedLines);
}

void Game::DrawText(::MiniKit::Engine::Context& context, ::MiniKit::Graphics::DrawInfo& drawSurface,
                    ::MiniKit::Graphics::CommandBuffer& commandBuffer, const ::MiniKit::Graphics::Color& color,
                    const std::string text, float& x, float& y, float width, float height) noexcept
{
    for (const char c : text) {
        if (!isalpha(c)) {
            continue;
        }
        char C = std::toupper(c);
        auto image = m_Images[std::string {C}];

        drawSurface.position.y = y;
        drawSurface.position.x = x;
        drawSurface.tint = color;
        drawSurface.scale = {width / image->GetSize().width, height / image->GetSize().height};

        commandBuffer.SetImage(*image.get());
        commandBuffer.Draw(drawSurface);

        x += width;
    }
}

void Game::DrawLogo(::MiniKit::Engine::Context& context, ::MiniKit::Graphics::DrawInfo& drawSurface,
                      ::MiniKit::Graphics::CommandBuffer& commandBuffer) noexcept
{   
    auto logoHeight = 60.0f + 20.f;
    auto logoWidth = 50.0f * 6 + 20.0f;

    //get center position from free space
    auto sideBarCenterX = ((0.5f * context.GetWindow().GetDrawableWidth() - g_Padding) + (-0.5f * context.GetWindow().GetDrawableWidth() + m_BlockScale.x * m_Images["field"]->GetSize().width * g_FieldWidth + 2 * g_Padding)) / 2;
    auto startX = sideBarCenterX - (4 * 40.0f + g_Padding + logoWidth) / 2;
    
    DrawText(context, drawSurface, commandBuffer, g_WhiteColor,
            "ucode", startX, m_LogoY, 40, 50);
    
    // square image
    drawSurface.position.y = m_LogoY;
    drawSurface.position.x = startX - 0.5f * 50.0f + g_Padding + 0.5f * logoWidth;
    drawSurface.tint = g_OrangeColor;
    drawSurface.scale = {logoWidth / m_Images["border"]->GetSize().width, logoHeight / m_Images["border"]->GetSize().height};

    commandBuffer.SetImage(*m_Images["border"]);
    commandBuffer.Draw(drawSurface);
    //
    startX += g_Padding + 10;

    DrawText(context, drawSurface, commandBuffer, g_BlackColor,
            "puzzle", startX, m_LogoY, 50, 60);

    startX = sideBarCenterX - (3 * 50.0f) / 2;
    DrawText(context, drawSurface, commandBuffer, g_OrangeColor,
            "next", startX, m_NextTetrominoTitleY, 50, 60);
    
    startX = sideBarCenterX - (4 * 50.0f) / 2;
    DrawText(context, drawSurface, commandBuffer, g_OrangeColor,
            "score", startX, m_ScoreTitleY, 50, 60);

    startX = sideBarCenterX - (4 * 50.0f) / 2;
    DrawText(context, drawSurface, commandBuffer, g_OrangeColor,
            "level", startX, m_LevelTitleY, 50, 60);

    startX = sideBarCenterX - (4 * 50.0f) / 2;
    DrawText(context, drawSurface, commandBuffer, g_OrangeColor,
            "lines", startX, m_LinesTitleY, 50, 60);
}

void Game::Draw(::MiniKit::Engine::Context& context) noexcept
{
    //drawing
    auto& graphicsDevice = context.GetGraphicsDevice();
    auto& commandBuffer = graphicsDevice.BeginFrame(1.0f, 1.0f, 1.0f, 1.0f);
    ::MiniKit::Graphics::DrawInfo drawSurface{};

    DrawBackground(context, drawSurface, commandBuffer);

    DrawLogo(context, drawSurface, commandBuffer);
    
    DrawField(context, drawSurface, commandBuffer);

    DrawBlocks(context, drawSurface, commandBuffer);

    DrawScore(context, drawSurface, commandBuffer);


    // static int counter = 0;
    // ImGui::Begin("Hello, world!");     
    // ImGui::SameLine();
    // ImGui::Text("counter = %d", counter);

    // ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    // ImGui::End();

    graphicsDevice.EndFrame(commandBuffer);
}

void Game::ChangeState(States state) noexcept
{
    m_State = state;
    m_States[state]->Enter();
}

bool Game::CheckCollision(Tetromino* tetromino) {
    for (size_t y = 0; y < tetromino->m_Shape.size(); y++) {
        for (size_t x = 0; x < tetromino->m_Shape[y].size(); x++) {
            int expectedX = tetromino->m_X + static_cast<int> (x);
            int expectedY = tetromino->m_Y + static_cast<int> (y) + 1;

            if (expectedY < 0) {
                continue;
            }

            if (tetromino->m_Shape[y][x] &&
                (expectedY >= g_FieldHeight || m_Blocks[expectedY][expectedX])) {
                return true;
            }
        }
    }

    return false;
}

bool Game::CheckSideCollision(int step) {
    for (size_t y = 0; y < m_Tetromino->m_Shape.size(); y++) {
        for (size_t x = 0; x < m_Tetromino->m_Shape[y].size(); x++) {
            if (m_Tetromino->m_Shape[y][x]) {
                int expectedX = m_Tetromino->m_X + static_cast<int> (x) + step;
                int expectedY = m_Tetromino->m_Y + static_cast<int> (y);
                
                if (expectedX < 0 || expectedX >= g_FieldWidth || expectedY >= g_FieldHeight ||
                    (expectedY >= 0 && m_Blocks[expectedY][expectedX])) {
                    return true;
                }
            }
        }
    }

    return false;
}

void Game::MoveSide(int step) {
    if (!CheckSideCollision(step)) {
        m_Tetromino->MoveSide(step);
    }
}

void Game::KeyDown(::MiniKit::Platform::Window& window, const ::MiniKit::Platform::KeyEvent& event) noexcept
{
    using ::MiniKit::Platform::Keycode;

    switch (event.keycode)
    {
        case Keycode::KeyLeft:
        case Keycode::KeyRight:
        case Keycode::KeyDown:
        case Keycode::KeyUp:
        case Keycode::KeyZ:
        case Keycode::KeyC:
        case Keycode::KeySpace:
        {
            if (!m_KeyState[event.keycode])
            {
                m_KeyState[event.keycode] = true;
                m_States[m_State]->KeyDown(event);
            }
            break;
        }
        default:
            break;
    }
 
}

void Game::KeyUp(::MiniKit::Platform::Window& window, const ::MiniKit::Platform::KeyEvent& event) noexcept
{
    using ::MiniKit::Platform::Keycode;

    switch (event.keycode)
    {
        case Keycode::KeyLeft:
        case Keycode::KeyRight:
        case Keycode::KeyDown:
        case Keycode::KeyUp:
        case Keycode::KeyZ:
        case Keycode::KeyC:
        case Keycode::KeySpace:
        {
            if (m_KeyState[event.keycode])
            {
                m_KeyState[event.keycode] = false;
                m_States[m_State]->KeyUp(event);
            }
            break;
        }
        default:
            break;
    }

}

GridResolver::GridResolver(std::shared_ptr<Game> game) : GameObject(game)
{

}

void GridResolver::React(const GameEvent& event) noexcept
{   
    auto game = m_game.lock();
    std::vector<int> compleatedLines;

    for (int line : event.lines) {
        bool compleated = true;

        for (auto& cell : game->m_Blocks[line]) {
            if (cell == nullptr) {
                compleated = false;
            }
        }
        if (compleated) {
            compleatedLines.push_back(line);
        }
    }
    if (!compleatedLines.empty()) {
        GameEvent new_event;
        new_event.eventType = EventType::LINES_COMPLETED_EVENT;
        new_event.lines = compleatedLines;
        game->m_EventSystem->AddEvent(std::move(new_event));
    }
}

GridManager::GridManager(std::shared_ptr<Game> game) : GameObject(game)
{

}

void GridManager::React(const GameEvent& event) noexcept
{   
    m_compleatedLines = event.lines;
}

void GridManager::ClearLines() noexcept {
    auto game = m_game.lock();

    for (int line : m_compleatedLines) {
        for (size_t x = 0; x < game->m_Blocks[line].size(); x++) {
            game->m_Blocks[line][x] = nullptr;
        }
    }
    for (int y = m_compleatedLines.back() - 1; y >= 0; y--) {
        for (int x = 0; x < g_FieldWidth; x++) {
            if (game->m_Blocks[y][x]) {
                game->m_Blocks[y + m_compleatedLines.size()][x] = std::make_unique<Block> ();
                game->m_Blocks[y + m_compleatedLines.size()][x]->color = game->m_Blocks[y][x]->color;
                game->m_Blocks[y][x] = nullptr;
            }
        }
    }
    m_compleatedLines.clear();
}

std::vector<int> GridManager::GetCompleatedLines() noexcept
{
    return m_compleatedLines;
}

void GridManager::AddToField() noexcept
{   
    auto game = m_game.lock();

    game->m_Tetromino->m_Color.alpha = 1.0f;
    for (size_t y = 0; y < game->m_Tetromino->m_Shape.size(); y++) {
        for (size_t x = 0; x < game->m_Tetromino->m_Shape[y].size(); x++) {
            if (game->m_Tetromino->m_Shape[y][x]) {
                if (game->m_Tetromino->m_Y + static_cast<int> (y) >= 0 &&
                    !game->m_Blocks[game->m_Tetromino->m_Y + y][game->m_Tetromino->m_X + x]) {
                    game->m_Blocks[game->m_Tetromino->m_Y + y][game->m_Tetromino->m_X + x] = std::make_unique<Block>();
                    game->m_Blocks[game->m_Tetromino->m_Y + y][game->m_Tetromino->m_X + x]->color = game->m_Tetromino->m_Color;
                }
            }
        }
    }

    GameEvent event;

    event.eventType = EventType::BLOCK_SET_EVENT;
    for (size_t y = 0; y < game->m_Tetromino->m_Shape.size(); y++) {
        if (static_cast<int> (y) + game->m_Tetromino->m_Y >= 0 && static_cast<int> (y) + game->m_Tetromino->m_Y < g_FieldHeight) { 
            event.lines.push_back(y + game->m_Tetromino->m_Y);
        }
    }
    game->m_EventSystem->AddEvent(std::move(event));

    game->m_Tetromino = nullptr;
    game->m_TetrominoGhost = nullptr;
}

void GridManager::ClearField() noexcept
{   
    auto game = m_game.lock();

    if (!game) {
        return;
    }

    for (auto& row : game->m_Blocks) {
        for (auto& col : row) {
            col = nullptr;
        }
    }
}

ScoreManager::ScoreManager(std::shared_ptr<Game> game) : GameObject(game)
{

}

void ScoreManager::React(const GameEvent& event) noexcept
{
    m_compleatedLines = event.lines.size();
}

void ScoreManager::AddtoScore() noexcept
{   
    auto game = m_game.lock();

    game->m_Score += 100 * m_compleatedLines + (10 * m_compleatedLines * game->m_Level);
    game->m_ClearedLines += m_compleatedLines;
    if (game->m_ClearedLines > 4 + 4 * game->m_Level) {
        game->m_Level++;
        game->m_FallSpeed *= 0.9f - 1 / (game->m_Level + 1);
    }
    m_compleatedLines = 0;
}

void ScoreManager::ClearScore() noexcept
{   
    auto game = m_game.lock();

    game->m_Score = 0;
    game->m_Level = 0;
    game->m_ClearedLines = 0;
    m_compleatedLines = 0;
}
