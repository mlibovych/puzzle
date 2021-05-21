#pragma once

#include <array>
#include <chrono>
#include <iostream>
#include <optional>
#include <memory>
#include <random>
#include <vector>
#include <unordered_map>

#include <ElementWithButtons.h>

#include <MiniKit/MiniKit.hpp>

class Game;
class Button;

enum class States
{
    NEW_GAME,
    SPAWN,
    POSITIONING,
    LINE_COMPLEATED,
    PAUSE,
    GAME_OVER,
    COUNT
};

class StateMachine
{   
    friend class Game;
protected:
    std::weak_ptr<Game> m_Game;
public:
    StateMachine(std::shared_ptr<Game> game);
    virtual ~StateMachine();

    virtual void Tick(::MiniKit::Engine::Context& context, ::MiniKit::Graphics::DrawInfo& drawSurface, ::MiniKit::Graphics::CommandBuffer& commandBuffer) noexcept = 0;
    virtual void KeyDown(const ::MiniKit::Platform::KeyEvent& event) noexcept = 0;
    virtual void KeyUp(const ::MiniKit::Platform::KeyEvent& event) noexcept = 0;
    virtual void Enter() noexcept = 0;
    virtual void Exit() noexcept = 0;
};

class GameState : public StateMachine
{
    
public:
    GameState(std::shared_ptr<Game> game);
    ~GameState();

    void Tick(::MiniKit::Engine::Context& context, ::MiniKit::Graphics::DrawInfo& drawSurface, ::MiniKit::Graphics::CommandBuffer& commandBuffer) noexcept override;
    virtual void KeyDown(const ::MiniKit::Platform::KeyEvent& event) noexcept override;
    virtual void KeyUp(const ::MiniKit::Platform::KeyEvent& event) noexcept override;
    virtual void Enter() noexcept override
    {

    }
    virtual void Exit() noexcept override
    {

    }
};

class NewGameState : public GameState 
{
    std::random_device m_Random;
public:
    NewGameState(std::shared_ptr<Game> game);
    ~NewGameState();

    virtual void Tick(::MiniKit::Engine::Context& context, ::MiniKit::Graphics::DrawInfo& drawSurface, ::MiniKit::Graphics::CommandBuffer& commandBuffer) noexcept override;
    virtual void Enter() noexcept override;
};
