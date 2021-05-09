#include <Game.h>
#include <imgui.h>

::std::error_code Game::Start(::MiniKit::Engine::Context& context) noexcept
{   
    //states
    m_States[States::SPAWN] = std::make_unique<SpawnState> (shared_from_this());
    // m_States[States::POSITIONING] = std::make_unique<PositioningState> (shared_from_this());
    m_State = m_States[States::SPAWN].get();

    //window
    auto& window = context.GetWindow();
    window.AddResponder(*this);

    //playing elements
    //images
    auto& graphicsDevice = context.GetGraphicsDevice();

    m_Images["block"] = graphicsDevice.CreateImage(g_BlockPath);
    m_Images["field"] = graphicsDevice.CreateImage(g_BackPath);
    
    //field data
    for (auto& row : m_Field) {
        for (auto& col : row) {
            col = nullptr;
        // row.fill(nullptr);
        }
    }
    //field background
    const auto& imageSize = m_Images["field"]->GetSize();

    const auto drawableWidth = static_cast<float>(context.GetWindow().GetDrawableWidth());
    const auto drawableHeight = static_cast<float>(context.GetWindow().GetDrawableHeight());
    const auto imagesGridPixelsWidth = g_FieldWidth * g_BlockWidth;
    const auto imagesGridPixelsHeight = g_FieldHeight * imageSize.width;
    const auto imageScaleY = static_cast<float>(drawableHeight - g_Padding * 2) / static_cast<float>(imagesGridPixelsHeight);
    m_BlockSkale = {imageScaleY, imageScaleY};

    const auto anchorPositionX =  -0.5f * drawableWidth + imageScaleY * imageSize.width / 2 + g_Padding;
    const auto anchorPositionY =  0.5f * drawableHeight - imageScaleY * imageSize.width / 2 - g_Padding;

    for (int y = g_FieldHeight - 1; y >= 0; y--) {
        for (int x = 0; x < g_FieldWidth; x++) {
            auto& cell = m_Background[y][x];

            cell.imageName = "field";
            cell.position.x = anchorPositionX + imageScaleY * imageSize.width * x;
            cell.position.y = anchorPositionY - imageScaleY * imageSize.width * y;
        }
    }

    return {};
}

::std::error_code Game::Shutdown(::MiniKit::Engine::Context& context) noexcept
{
    return {};
}

void Game::Tick(::MiniKit::Engine::Context& context) noexcept
{   
    m_State->Tick(context);
}

void Game::DrawField(::MiniKit::Engine::Context& context) noexcept
{
    //moving
    uint64_t now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    
    if (!m_Tetromino) {
        m_Tetromino = ::std::make_unique<Tetromino> ();
    }
    if (now - m_FrameTime > 200) {
        m_FrameTime = now;
        m_Tetromino->moveDown();
    }

    //check state
    try {
        for (size_t y = 0; y < m_Tetromino->m_Shape.size(); y++) {
            for (size_t x = 0; x < m_Tetromino->m_Shape[y].size(); x++) {
                if (m_Tetromino->m_Shape[y][x] &&
                    (m_Tetromino->m_Y + y >= g_FieldHeight || m_Field[m_Tetromino->m_Y + y][m_Tetromino->m_X + x])) {
                    throw true;
                }
            }
        }
    }
    catch (bool) {
        addToField();
        return;
    }

    //drawing
    auto& graphicsDevice = context.GetGraphicsDevice();
    auto& commandBuffer = graphicsDevice.BeginFrame(1.0f, 1.0f, 1.0f, 1.0f);
    
    ::MiniKit::Graphics::DrawInfo drawSurface{};
    commandBuffer.SetImage(*m_Images["field"]);
    
    //draw field
    for (const auto& row : m_Background) {
        for (const auto& col : row) {
            drawSurface.tint = col.color;
            drawSurface.position = col.position;
            drawSurface.scale = m_BlockSkale;

            commandBuffer.Draw(drawSurface);
        }
    }

    //draw tetromino
    commandBuffer.SetImage(*m_Images["block"]);

    for (size_t y = 0; y < m_Tetromino->m_Shape.size(); y++) {
        for (size_t x = 0; x < m_Tetromino->m_Shape[y].size(); x++) {
            if (m_Tetromino->m_Shape[y][x]) {
                drawSurface.tint = m_Tetromino->m_Color;
                drawSurface.position.x = m_Background[m_Tetromino->m_Y + y][m_Tetromino->m_X + x].position.x;
                drawSurface.position.y = m_Background[m_Tetromino->m_Y + y][m_Tetromino->m_X + x].position.y;
                drawSurface.scale = m_BlockSkale;

                commandBuffer.Draw(drawSurface);
            }
        }
    }

    //draw blocks
    for (size_t y = 0; y < m_Field.size(); y++) {
        for (size_t x = 0; x < m_Field[y].size(); x++) {
            if (m_Field[y][x]) {
                drawSurface.tint = m_Field[y][x]->color;
                drawSurface.position = m_Background[y][x].position;
                drawSurface.scale = m_BlockSkale;

                commandBuffer.Draw(drawSurface);
            }
        }
    }

    graphicsDevice.EndFrame(commandBuffer);
}

void Game::addToField() noexcept {
    for (size_t y = 0; y < m_Tetromino->m_Shape.size(); y++) {
        for (size_t x = 0; x < m_Tetromino->m_Shape[y].size(); x++) {
            if (m_Tetromino->m_Shape[y][x] && m_Tetromino->m_Y + y > 0) {
                m_Field[m_Tetromino->m_Y + y - 1][m_Tetromino->m_X + x] = std::make_unique<Block>();
                m_Field[m_Tetromino->m_Y + y - 1][m_Tetromino->m_X + x]->color = m_Tetromino->m_Color;
            }
        }
    }
    m_Tetromino = nullptr;
}
