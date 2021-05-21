#pragma once

#include <StateMachine.h>

class LineCompleatedState : public GameState
{
    float m_Value { 0.0f };
public:
    LineCompleatedState(std::shared_ptr<Game> game);
    ~LineCompleatedState();

    virtual void Tick(::MiniKit::Engine::Context& context, ::MiniKit::Graphics::DrawInfo& drawSurface, ::MiniKit::Graphics::CommandBuffer& commandBuffer) noexcept override;
    virtual void Enter() noexcept override;
    void GetColor(::MiniKit::Graphics::Color& color, float delta) noexcept;
    float GetAlpha() noexcept;
};