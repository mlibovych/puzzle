#include <GameOverState.h>
#include <Game.h>

using ::MiniKit::Platform::Keycode;

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
