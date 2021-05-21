#pragma once

#include <StateMachine.h>

class SpawnState : public GameState
{
    std::random_device m_Random;
public:
    SpawnState(std::shared_ptr<Game> game);
    ~SpawnState();

    virtual void Tick(::MiniKit::Engine::Context& context, ::MiniKit::Graphics::DrawInfo& drawSurface, ::MiniKit::Graphics::CommandBuffer& commandBuffer) noexcept override;
    virtual void Enter() noexcept override;
};