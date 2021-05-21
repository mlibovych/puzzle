#pragma once

#include <StateMachine.h>

class GameOverState : public GameState, public ElementWithButtons
{
    std::string m_Invitation { "Enter your name" };
    std::string m_Name { };
    bool m_Save { true };
public:
    GameOverState(std::shared_ptr<Game> game);
    ~GameOverState();

    virtual void Tick(::MiniKit::Engine::Context& context, ::MiniKit::Graphics::DrawInfo& drawSurface, ::MiniKit::Graphics::CommandBuffer& commandBuffer) noexcept override;
    virtual void Enter() noexcept override;
    virtual void KeyDown(const ::MiniKit::Platform::KeyEvent& event) noexcept override;
};