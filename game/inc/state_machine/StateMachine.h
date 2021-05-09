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

class Game;

struct alignas(16) Block {
    ::MiniKit::Graphics::Color color{ 1.0f, 1.0f, 1.0f, 1.0f };
    // size_t y;
    // size_t x;
};

struct alignas(16) SpriteEntity
{
    ::MiniKit::Graphics::Color color{ 1.0f, 1.0f, 1.0f, 1.0f };
    ::MiniKit::Graphics::float2 position{ 0.0f, 0.0f };

    ::std::string imageName;
};

enum class States
{
    NEW_GAME,
    SPAWN,
    POSITIONING,
    LINE_COMPLEATED,
    PAUSE,
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
};

class GameState : public StateMachine
{
public:
    GameState(std::shared_ptr<Game> game);
    ~GameState();

    virtual void Tick(::MiniKit::Engine::Context& context) noexcept override;
};

class SpawnState : public GameState {
public:
    SpawnState(std::shared_ptr<Game> game);
    ~SpawnState();

    virtual void Tick(::MiniKit::Engine::Context& context) noexcept override;
};

// class PositioningState : public GameState {
// public:
//     PositioningState(std::shared_ptr<Game> game);
//     ~PositioningState();

//     virtual void Tick() noexcept override;
// };

// class LineCompleatedState : public GameState {
// public:
// };

// class PauseState : public GameState {
// public:
// };
