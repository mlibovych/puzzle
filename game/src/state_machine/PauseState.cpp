#include <PauseState.h>
#include <Game.h>

using ::MiniKit::Platform::Keycode;

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
