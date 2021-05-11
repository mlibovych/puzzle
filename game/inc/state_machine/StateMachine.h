#pragma once

#include <array>
#include <chrono>
#include <iostream>
#include <optional>
#include <memory>
#include <vector>
#include <unordered_map>

#include <MiniKit/MiniKit.hpp>
#include <Tetromino.h>

constexpr int g_FieldWidth = 10;
constexpr int g_FieldHeight = 20;

constexpr int g_SideMovingSpeed = 100;

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
    std::weak_ptr<Game> m_game;
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

    void Tick(::MiniKit::Engine::Context& context) noexcept;
    virtual void KeyDown(const ::MiniKit::Platform::KeyEvent& event) noexcept override;
    virtual void KeyUp(const ::MiniKit::Platform::KeyEvent& event) noexcept override;
    virtual void Enter() noexcept override
    {

    }
};

class SpawnState : public GameState {
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
};

class LineCompleatedState : public GameState {
public:
    LineCompleatedState(std::shared_ptr<Game> game);
    ~LineCompleatedState();

    virtual void Tick(::MiniKit::Engine::Context& context) noexcept override;
    virtual void Enter() noexcept override;
};

