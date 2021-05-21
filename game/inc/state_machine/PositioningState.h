#pragma once

#include <StateMachine.h>

enum class Direction
{
    LEFT,
    RIGHT
};

class PositioningState : public GameState
{
    float m_DownValue { 0.0f };
    float m_SideValue { 0.0f };
    float m_LockValue { 0.0f };

    bool m_Lock = false;
    bool m_SoftDrop = false;

    std::unordered_map<Direction, int> m_DirectionsQueue;
    std::unordered_map<Direction, int> m_DirectionStep;
public:
    PositioningState(std::shared_ptr<Game> game);
    ~PositioningState();

    virtual void Tick(::MiniKit::Engine::Context& context, ::MiniKit::Graphics::DrawInfo& drawSurface, ::MiniKit::Graphics::CommandBuffer& commandBuffer) noexcept override;
    virtual void KeyDown(const ::MiniKit::Platform::KeyEvent& event) noexcept override;
    virtual void KeyUp(const ::MiniKit::Platform::KeyEvent& event) noexcept override;

    void Start(Direction direction) noexcept;
    void Stop(Direction direction) noexcept;

    virtual void Enter() noexcept override;
    void RotateRight() noexcept;
    void RotateLeft() noexcept;
    void HardDrop() noexcept;
    void SoftDrop(bool drop) noexcept;
};