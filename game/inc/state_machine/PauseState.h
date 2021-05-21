#pragma once

#include <StateMachine.h>

class PauseState : public GameState {
public:
    PauseState(std::shared_ptr<Game> game);
    ~PauseState();

    virtual void Tick(::MiniKit::Engine::Context& context, ::MiniKit::Graphics::DrawInfo& drawSurface, ::MiniKit::Graphics::CommandBuffer& commandBuffer) noexcept override;
    virtual void KeyDown(const ::MiniKit::Platform::KeyEvent& event) noexcept override;
    virtual void Enter() noexcept override;
    virtual void Exit() noexcept override;
};