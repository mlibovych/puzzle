#pragma once

#include <array>
#include <chrono>
#include <iostream>
#include <optional>
#include <memory>
#include <random>
#include <vector>
#include <unordered_map>

#include <MiniKit/MiniKit.hpp>

class Game;

enum class States
{
    NEW_GAME,
    SPAWN,
    POSITIONING,
    LINE_COMPLEATED,
    PAUSE,
    GAME_OVER,
    COUT
};

class StateMachine
{   
    friend class Game;
protected:
    std::weak_ptr<Game> m_Game;
public:
    StateMachine(std::shared_ptr<Game> game);
    virtual ~StateMachine();

    virtual void Tick(::MiniKit::Engine::Context& context) noexcept = 0;
    virtual void KeyDown(const ::MiniKit::Platform::KeyEvent& event) noexcept = 0;
    virtual void KeyUp(const ::MiniKit::Platform::KeyEvent& event) noexcept = 0;
    virtual void Enter() noexcept = 0;
};

class GameState : public StateMachine
{
    
public:
    GameState(std::shared_ptr<Game> game);
    ~GameState();

    void Tick(::MiniKit::Engine::Context& context) noexcept override;
    virtual void KeyDown(const ::MiniKit::Platform::KeyEvent& event) noexcept override;
    virtual void KeyUp(const ::MiniKit::Platform::KeyEvent& event) noexcept override;
    virtual void Enter() noexcept override
    {

    }
};

class SpawnState : public GameState {
    std::random_device m_Random;
public:
    SpawnState(std::shared_ptr<Game> game);
    ~SpawnState();

    virtual void Tick(::MiniKit::Engine::Context& context) noexcept override;
    virtual void Enter() noexcept override;
};

enum class Direction
{
    LEFT,
    RIGHT
};

class PositioningState : public GameState {
    float m_DownValue { 0.0f };
    float m_SideValue { 0.0f };
    float m_LockValue { 0.0f };

    bool m_Lock = false;

    std::unordered_map<Direction, int> m_DirectionsQueue;
    std::unordered_map<Direction, int> m_DirectionStep;
public:
    PositioningState(std::shared_ptr<Game> game);
    ~PositioningState();

    virtual void Tick(::MiniKit::Engine::Context& context) noexcept override;
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

class PauseState : public GameState {
public:
    PauseState(std::shared_ptr<Game> game);
    ~PauseState();

    virtual void Tick(::MiniKit::Engine::Context& context) noexcept override;
    virtual void KeyDown(const ::MiniKit::Platform::KeyEvent& event) noexcept override;
    // virtual void KeyUp(const ::MiniKit::Platform::KeyEvent& event) noexcept override;
};

class LineCompleatedState : public GameState {
    float m_Value { 0.0f };
public:
    LineCompleatedState(std::shared_ptr<Game> game);
    ~LineCompleatedState();

    virtual void Tick(::MiniKit::Engine::Context& context) noexcept override;
    virtual void Enter() noexcept override;
    void GetColor(::MiniKit::Graphics::Color& color, float delta) noexcept;
    float GetAlpha() noexcept;
};

class NewGameState : public GameState {
    float m_Value { 0.0f };
    std::random_device m_Random;
public:
    NewGameState(std::shared_ptr<Game> game);
    ~NewGameState();

    virtual void Tick(::MiniKit::Engine::Context& context) noexcept override;
    virtual void Enter() noexcept override;
};

// class GameOverState : public GameState {
//     float m_Value { 0.0f };
//     std::random_device m_Random;
// public:
//     GameOverState(std::shared_ptr<Game> game);
//     ~GameOverState();

//     virtual void Tick(::MiniKit::Engine::Context& context) noexcept override;
//     virtual void Enter() noexcept override;
// };
