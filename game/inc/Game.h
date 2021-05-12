#pragma once

#include <array>
#include <chrono>
#include <iostream>
#include <memory>
#include <vector>
#include <unordered_map>

#include <StateMachine.h>
#include <EventSystem.h>

#include <MiniKit/MiniKit.hpp>

constexpr int g_FieldWidth = 10;
constexpr int g_FieldHeight = 20;

constexpr int g_BlockWidth = 80;
constexpr int g_AppWidth = 800;
constexpr int g_AppHeight = 900;
constexpr int g_Padding = 10;

constexpr float g_LineCompleatedAnimationTime = 0.5f;
const ::MiniKit::Graphics::Color g_LineCompleatedColor{ 0.9f, 0.4f, 0.1f, 1.0f };

const std::string g_BlockPath = "assets/sq.png"; 
const std::string g_BackPath = "assets/3.png";

class StateMachine;
class GameState;
class SpawnState;
class PositioningState;
class LineCompleatedState;

struct alignas(16) Block
{
    ::MiniKit::Graphics::Color color{ 1.0f, 1.0f, 1.0f, 1.0f };
};

struct alignas(16) SpriteEntity
{
    ::MiniKit::Graphics::Color color{ 1.0f, 1.0f, 1.0f, 1.0f };
    ::MiniKit::Graphics::float2 position{ 0.0f, 0.0f };
};

class GameObject : public Object
{
protected:
    std::weak_ptr<Game> m_game;
public:
    GameObject(std::shared_ptr<Game> game) : m_game(game)
    {

    }

    virtual void react(const GameEvent& event) = 0;
};


class GridResolver : public GameObject
{
public:
    GridResolver(std::shared_ptr<Game> game);

    virtual void react(const GameEvent& event) noexcept;
};

class GridManager : public GameObject
{
    std::vector<int> m_compleatedLines;
public:
    GridManager(std::shared_ptr<Game> game);

    virtual void react(const GameEvent& event) noexcept;

    void AddToField() noexcept;
    void ClearLines() noexcept;
    std::vector<int>& GetCompleatedLines() noexcept;
};

class Game final : public ::MiniKit::Engine::Application, public ::MiniKit::Platform::Responder,
                   public std::enable_shared_from_this<Game>
{
    friend class GameState;
    friend class SpawnState;
    friend class PositioningState;
    friend class LineCompleatedState;

    friend class GridResolver;
    friend class GridManager;

    States m_State;
    ::std::unordered_map<States, ::std::unique_ptr<StateMachine>> m_States;

    ::std::unique_ptr<EventSystem> m_EventSystem { nullptr };
    ::std::unique_ptr<GridResolver> m_GridResolver { nullptr };
    ::std::unique_ptr<GridManager> m_GridManager { nullptr };

    ::std::unordered_map<::std::string, ::std::shared_ptr<::MiniKit::Graphics::Image>> m_Images;

    ::std::array<::std::array<::std::unique_ptr<Block>, g_FieldWidth>, g_FieldHeight> m_Field;
    ::std::array<::std::array<SpriteEntity, g_FieldWidth>, g_FieldHeight> m_Background;
    ::std::unique_ptr<Tetromino> m_Tetromino { nullptr };
    ::std::unique_ptr<Tetromino> m_TetrominoGhost { nullptr };
    uint64_t m_FrameTime;
    uint64_t m_SideMovingTime;

    ::MiniKit::Graphics::float2 m_BlockSkale{ 1.0f, 1.0f };

    int m_FallSpeed = 200;
    int m_SideSpeed = 100;

    std::unordered_map<MiniKit::Platform::Keycode, bool> m_KeyState = {};
    
    void DrawField(::MiniKit::Engine::Context& context) noexcept;
    void DrawBackground(::MiniKit::Engine::Context& context, ::MiniKit::Graphics::DrawInfo& drawSurface, ::MiniKit::Graphics::CommandBuffer& commandBuffer) noexcept;
    void DrawBlocks(::MiniKit::Engine::Context& context, ::MiniKit::Graphics::DrawInfo& drawSurface, ::MiniKit::Graphics::CommandBuffer& commandBuffer) noexcept;
    void ChangeState(States state) noexcept;
    void CheckCollision(Tetromino* tetromino);
    void CheckSideCollision(int step);
    void MoveSide(int step);
    void GetGhostPosition();
public:
    ::std::error_code Start(::MiniKit::Engine::Context& context) noexcept override;
    
    ::std::error_code Shutdown(::MiniKit::Engine::Context& context) noexcept override;
    
    void Tick(::MiniKit::Engine::Context& context) noexcept override;

    void KeyDown(::MiniKit::Platform::Window& window, const ::MiniKit::Platform::KeyEvent& event) noexcept override;
    
    void KeyUp(::MiniKit::Platform::Window& window, const ::MiniKit::Platform::KeyEvent& event) noexcept override;
};