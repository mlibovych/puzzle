#include <Game.h>
#include <imgui.h>

::std::error_code Game::Start(::MiniKit::Engine::Context& context) noexcept
{   
    //window
    auto& window = context.GetWindow();
    window.AddResponder(*this);
    //images
    auto& graphicsDevice = context.GetGraphicsDevice();

    m_Images["block"] = graphicsDevice.CreateImage(g_BlockPath);
    m_Images["field"] = graphicsDevice.CreateImage(g_BackPath);

    if (!m_Images["block"] || !m_Images["field"])
    {
        return ::MiniKit::MakeErrorCode(::MiniKit::Error::InitializationFailed);
    }
    //field data
    for (auto& row : m_Field) {
        row.fill(0);
    }
    //field background
    const auto& imageSize = m_Images["field"]->GetSize();

    const auto drawableWidth = static_cast<float>(context.GetWindow().GetDrawableWidth());
    const auto drawableHeight = static_cast<float>(context.GetWindow().GetDrawableHeight());
    const auto imagesGridPixelsWidth = 10 * g_BlockWidth;
    const auto imagesGridPixelsHeight = 20 * imageSize.width;
    const auto imageScaleY = static_cast<float>(drawableHeight - g_Padding * 2) / static_cast<float>(imagesGridPixelsHeight);

    const auto anchorPositionX =  -0.5f * drawableWidth + imageScaleY * imageSize.width / 2 + g_Padding;
    const auto anchorPositionY =  0.5f * drawableHeight - imageScaleY * imageSize.width / 2 - g_Padding;

    for (int y = g_FieldHeight - 1; y >= 0; y--) {
        for (int x = 0; x < g_FieldWidth; x++) {
            auto& cell = m_Background[y][x];

            cell.imageName = "field";
            cell.position.x = anchorPositionX + imageScaleY * imageSize.width * x;
            cell.position.y = anchorPositionY - imageScaleY * imageSize.width * y;
            cell.scale = {imageScaleY, imageScaleY};
        }
    }
    // auto y = 0.0f;
    // for (const auto& row : m_Field) {
    //     auto x = 0.0f;
    //     for (const auto& col : row) {
    //         if (!col) {
    //             drawStaticLogo.position.x = anchorPositionX + imageScaleY * m_BackImage->GetSize().width * x;
    //             drawStaticLogo.position.y = anchorPositionY - imageScaleY * m_BackImage->GetSize().width * y;
    //             drawStaticLogo.scale = { imageScaleY, imageScaleY };

    //             drawStaticLogo.tint.alpha = 0.8f;
    //             // drawStaticLogo.tint.blue = 0.0f;
    //             // drawStaticLogo.tint.red = 0.0f;
    //             // drawStaticLogo.tint.green = 0.0f;
                
    //             commandBuffer.Draw(drawStaticLogo);
    //         }
    //         x++;
    //     }
    //     y++;
    // }

    return {};
}

::std::error_code Game::Shutdown(::MiniKit::Engine::Context& context) noexcept
{
    return {};
}

void Game::Tick(::MiniKit::Engine::Context& context) noexcept
{   
    //moving
    // uint64_t now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    
    // if (now - m_FrameTime > 200) {
    //     m_FrameTime = now;
    //     m_Tetromino->m_Y--;
    // }

    //drawing
    auto& graphicsDevice = context.GetGraphicsDevice();
    auto& commandBuffer = graphicsDevice.BeginFrame(1.0f, 1.0f, 1.0f, 1.0f);
    
    ::MiniKit::Graphics::DrawInfo drawSurface{};
    commandBuffer.SetImage(*m_Images["field"]);

    for (const auto& row : m_Background) {
        for (const auto& col : row) {
            drawSurface.tint = col.color;
            drawSurface.position = col.position;
            drawSurface.scale = col.scale;

            commandBuffer.Draw(drawSurface);
        }
    }
    //fps
    // ::ImGui::Begin("Info");
    // ::ImGui::Text("FPS: %.3f frames/second", 1.0f / context.GetFrameDelta());
    // ::ImGui::Text("Press ESC to exit.");
    // ::ImGui::End();

    graphicsDevice.EndFrame(commandBuffer);

    // const auto drawableWidth = static_cast<float>(context.GetWindow().GetDrawableWidth());
    // const auto drawableHeight = static_cast<float>(context.GetWindow().GetDrawableHeight());
    // const auto imagesGridPixelsWidth = 10 * g_BlockWidth;
    // const auto imagesGridPixelsHeight = 20 * m_BackImage->GetSize().width;
    // const auto imageScaleX = static_cast<float> (g_BlockWidth) / m_BackImage->GetSize().width;
    // const auto imageScaleY = static_cast<float>(drawableHeight - g_Padding * 2) / static_cast<float>(imagesGridPixelsHeight);
    // {   
    //     commandBuffer.SetImage(*m_BackImage);
    //     auto width = m_BackImage->GetSize().width / 5;
    //     const auto anchorPositionX =  -0.5f * drawableWidth + imageScaleY * m_BackImage->GetSize().width / 2 + g_Padding;
    //     const auto anchorPositionY =  0.5f * drawableHeight - imageScaleY * m_BackImage->GetSize().width / 2 - g_Padding;

    //     auto y = 0.0f;
    //     for (const auto& row : m_Field) {
    //         auto x = 0.0f;
    //         for (const auto& col : row) {
    //             if (!col) {
    //                 drawStaticLogo.position.x = anchorPositionX + imageScaleY * m_BackImage->GetSize().width * x;
    //                 drawStaticLogo.position.y = anchorPositionY - imageScaleY * m_BackImage->GetSize().width * y;
    //                 drawStaticLogo.scale = { imageScaleY, imageScaleY };

    //                 drawStaticLogo.tint.alpha = 0.8f;
    //                 // drawStaticLogo.tint.blue = 0.0f;
    //                 // drawStaticLogo.tint.red = 0.0f;
    //                 // drawStaticLogo.tint.green = 0.0f;
                    
    //                 commandBuffer.Draw(drawStaticLogo);
    //             }
    //             x++;
    //         }
    //         y++;
    //     }
    // }
}
