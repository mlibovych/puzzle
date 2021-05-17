#pragma once

#include <array>
#include <chrono>
#include <iostream>
#include <memory>
#include <vector>
#include <unordered_map>

#include <EventSystem.h>
#include <Tetromino.h>
#include <Settings.h>
#include <StateMachine.h>

#include <MiniKit/MiniKit.hpp>

constexpr int g_FieldWidth = 10;
constexpr int g_FieldHeight = 20;

constexpr int g_BlockWidth = 80;
constexpr int g_AppWidth = 800;
constexpr int g_AppHeight = 900;
constexpr int g_Padding = 10;

constexpr int g_NextTetrominoY = 250;

constexpr float g_FallSpeed = 0.600f;

constexpr float g_LineCompleatedAnimationTime = 0.5f;
const ::MiniKit::Graphics::Color g_LineCompleatedColor{ 0.9f, 0.4f, 0.1f, 1.0f };
const ::MiniKit::Graphics::Color g_OrangeColor { 1.0f, 0.38f, 0.05f, 1.0f };
const ::MiniKit::Graphics::Color g_BackgroundColor { 0.1f, 0.1f, 0.1f, 1.0f };
const ::MiniKit::Graphics::Color g_TextColor { 0.4f, 0.1f, 0.1f, 1.0f };
const ::MiniKit::Graphics::Color g_GhostColor { 1.0f, 0.55f, 0.22f, 0.5f };

const std::string g_BlockPath = "assets/sq.png"; 
const std::string g_FieldPath = "assets/back.png";
const std::string g_BackPath = "assets/pure.png";
const std::string g_BorderPath = "assets/border.png";

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

    virtual void React(const GameEvent& event) noexcept = 0;
};


class GridResolver : public GameObject
{
public:
    GridResolver(std::shared_ptr<Game> game);

    virtual void React(const GameEvent& event) noexcept;
};

class GridManager : public GameObject
{
    std::vector<int> m_compleatedLines;
public:
    GridManager(std::shared_ptr<Game> game);

    virtual void React(const GameEvent& event) noexcept;

    void AddToField() noexcept;
    void ClearLines() noexcept;
    void ClearField() noexcept;
    std::vector<int> GetCompleatedLines() noexcept;
};

class ScoreManager : public GameObject
{
    int m_compleatedLines;
public:
    ScoreManager(std::shared_ptr<Game> game);

    virtual void React(const GameEvent& event) noexcept;
    void AddtoScore() noexcept;
    void ClearScore() noexcept;
};

class Game final : public ::MiniKit::Engine::Application, public ::MiniKit::Platform::Responder,
                   public std::enable_shared_from_this<Game>
{
    friend class GameState;
    friend class SpawnState;
    friend class PositioningState;
    friend class LineCompleatedState;
    friend class NewGameState;
    friend class PauseState;

    friend class GridResolver;
    friend class GridManager;
    friend class ScoreManager;

    int m_Score { 0 };
    int m_ClearedLines { 0 };
    int m_Level { 0 };
    bool m_Ghost { true };

    ::std::unique_ptr<Settings> m_Settings;

    States m_State;
    ::std::unordered_map<States, ::std::unique_ptr<StateMachine>> m_States;

    ::std::unique_ptr<EventSystem> m_EventSystem { nullptr };
    ::std::unique_ptr<GridResolver> m_GridResolver { nullptr };
    ::std::unique_ptr<GridManager> m_GridManager { nullptr };
    ::std::unique_ptr<ScoreManager> m_ScoreManager { nullptr };

    ::std::unordered_map<::std::string, ::std::shared_ptr<::MiniKit::Graphics::Image>> m_Images;

    ::std::array<::std::array<::std::unique_ptr<Block>, g_FieldWidth>, g_FieldHeight> m_Blocks;
    ::std::array<::std::array<SpriteEntity, g_FieldWidth>, g_FieldHeight> m_Field;

    ::std::vector<::std::unique_ptr<Tetromino>> m_Tetrominos;
    ::std::unique_ptr<Tetromino> m_Tetromino { nullptr };
    ::std::unique_ptr<Tetromino> m_TetrominoGhost { nullptr };
    ::std::unique_ptr<Tetromino> m_NextTetromino { nullptr };
    int m_TetrominosFrequency {1};

    ::MiniKit::Graphics::float2 m_BlockScale{ 1.0f, 1.0f };
    ::MiniKit::Graphics::float2 m_BackgroundScale{ 1.0f, 1.0f };

    float m_FallSpeed { 0.600f };
    float m_SideSpeed { 0.100f };
    float m_LockDelay { 0.500f };

    float m_AnchorPositionY { 0.0f };
    float m_AnchorPositionX { 0.0f };

    float m_NextTetrominoX { 0.0f };
    float m_ScoreNumberY { 300.0f };
    float m_LevelNumberY { 100.0f };
    float m_LinesNumberY { -100.0f };

    std::unordered_map<MiniKit::Platform::Keycode, bool> m_KeyState {};
    
    void Draw(::MiniKit::Engine::Context& context) noexcept;
    void DrawBackground(::MiniKit::Engine::Context& context, ::MiniKit::Graphics::DrawInfo& drawSurface,::MiniKit::Graphics::CommandBuffer& commandBuffer) noexcept;
    void DrawField(::MiniKit::Engine::Context& context, ::MiniKit::Graphics::DrawInfo& drawSurface, ::MiniKit::Graphics::CommandBuffer& commandBuffer) noexcept;
    void DrawBlocks(::MiniKit::Engine::Context& context, ::MiniKit::Graphics::DrawInfo& drawSurface, ::MiniKit::Graphics::CommandBuffer& commandBuffer) noexcept;
    void DrawScore(::MiniKit::Engine::Context& context, ::MiniKit::Graphics::DrawInfo& drawSurface, ::MiniKit::Graphics::CommandBuffer& commandBuffer) noexcept;
    void DrawNumber(::MiniKit::Engine::Context& context, ::MiniKit::Graphics::DrawInfo& drawSurface,
                      ::MiniKit::Graphics::CommandBuffer& commandBuffer, int number) noexcept;
    void DrawLogo(::MiniKit::Engine::Context& context, ::MiniKit::Graphics::DrawInfo& drawSurface, ::MiniKit::Graphics::CommandBuffer& commandBuffer) noexcept;
    void ChangeState(States state) noexcept;
    bool CheckCollision(Tetromino* tetromino);
    bool CheckSideCollision(int step);
    void MoveSide(int step);
    void GetGhostPosition();

    void UpdateSettings();
public:
    ::std::error_code Start(::MiniKit::Engine::Context& context) noexcept override;
    
    ::std::error_code Shutdown(::MiniKit::Engine::Context& context) noexcept override;
    
    void Tick(::MiniKit::Engine::Context& context) noexcept override;

    void KeyDown(::MiniKit::Platform::Window& window, const ::MiniKit::Platform::KeyEvent& event) noexcept override;
    
    void KeyUp(::MiniKit::Platform::Window& window, const ::MiniKit::Platform::KeyEvent& event) noexcept override;
};