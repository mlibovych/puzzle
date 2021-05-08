#pragma once

#include <array>
#include <chrono>
#include <iostream>
#include <optional>
#include <memory>
#include <vector>
#include <unordered_map>

#include <MiniKit/MiniKit.hpp>

constexpr int g_FieldWidth = 10;
constexpr int g_FieldHeight = 20;

class Game;

struct alignas(16) SpriteEntity
{
    ::MiniKit::Graphics::Color color{ 1.0f, 1.0f, 1.0f, 1.0f };
    ::MiniKit::Graphics::float2 position{ 0.0f, 0.0f };
    ::MiniKit::Graphics::float2 scale{ 1.0f, 1.0f };

    ::std::string imageName;
};

enum class States
{
    MENU,
    GAME,
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
    ::std::unordered_map<::std::string, ::std::shared_ptr<::MiniKit::Graphics::Image>> m_Images;

    ::std::array<::std::array<int, g_FieldWidth>, g_FieldHeight> m_Field;
    ::std::array<::std::array<SpriteEntity, g_FieldWidth>, g_FieldHeight> m_Background;
    ::std::vector<SpriteEntity> m_Blocks;

    uint64_t m_FrameTime;
public:
    GameState(std::shared_ptr<Game> game, ::MiniKit::Engine::Context& context);
    ~GameState();

    virtual void Tick(::MiniKit::Engine::Context& context) noexcept override;
};
