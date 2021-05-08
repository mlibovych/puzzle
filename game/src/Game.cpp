#include <Game.h>
#include <imgui.h>

::std::error_code Game::Start(::MiniKit::Engine::Context& context) noexcept
{   
    //states
    m_States[States::GAME] = std::make_unique<GameState> (shared_from_this(), context);
    m_State = m_States[States::GAME].get();
    //window
    auto& window = context.GetWindow();
    window.AddResponder(*this);

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
