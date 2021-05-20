#pragma once

#include <array>
#include <chrono>
#include <deque>
#include <iostream>
#include <memory>
#include <vector>
#include <unordered_map>

#include <EventSystem.h>
#include <Tetromino.h>
#include <Settings.h>
#include <App.h>
#include <AppElement.h>
#include <StateMachine.h>
#include <ElementWithButtons.h>

#include <MiniKit/MiniKit.hpp>

class App;

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
    void AddtoScore(int amount) noexcept;
    void ClearScore() noexcept;
};

class Game : public AppElement, public std::enable_shared_from_this<Game>
{   
    friend class App;

    friend class GameState;
    friend class SpawnState;
    friend class PositioningState;
    friend class LineCompleatedState;
    friend class NewGameState;
    friend class PauseState;
    friend class GameOverState;

    friend class GridResolver;
    friend class GridManager;
    friend class ScoreManager;

    int m_Score { 0 };
    int m_ClearedLines { 0 };
    int m_Level { 0 };
    bool m_Ghost { true };
    bool m_Pause { false };

    ::std::unique_ptr<Settings> m_Settings;

    States m_State;
    ::std::unordered_map<States, ::std::unique_ptr<StateMachine>> m_States;

    ::std::unique_ptr<EventSystem> m_EventSystem { nullptr };
    ::std::unique_ptr<GridResolver> m_GridResolver { nullptr };
    ::std::unique_ptr<GridManager> m_GridManager { nullptr };
    ::std::unique_ptr<ScoreManager> m_ScoreManager { nullptr };

    ::std::array<::std::array<::std::unique_ptr<Block>, g_FieldWidth>, g_FieldHeight> m_Blocks;
    ::std::array<::std::array<SpriteEntity, g_FieldWidth>, g_FieldHeight> m_Field;

    ::std::vector<::std::unique_ptr<Tetromino>> m_Tetrominos;
    ::std::unique_ptr<Tetromino> m_Tetromino { nullptr };
    ::std::unique_ptr<Tetromino> m_TetrominoGhost { nullptr };
    ::std::unique_ptr<Tetromino> m_NextTetromino { nullptr };
    int m_TetrominosFrequency { 1 };

    ::MiniKit::Graphics::float2 m_BlockScale{ 1.0f, 1.0f };

    float m_FallSpeed { 0.600f };
    float m_SideSpeed { 0.100f };
    float m_LockDelay { 0.500f };


    float m_NextTetrominoX { 0.0f };
    float m_NextTetrominoY { 0.0f };
    float m_NextTetrominoTitleY { 0.0f };
     
    float m_LogoY { 0.0f };

    float m_ScoreTitleY { 0.0f };
    float m_LevelTitleY { 0.0f };
    float m_LinesTitleY { 0.0f };
    float m_ScoreNumberY { 0.0f };
    float m_LevelNumberY { 0.0f };
    float m_LinesNumberY { 0.0f };

    void Draw(::MiniKit::Engine::Context& context, ::MiniKit::Graphics::DrawInfo& drawSurface, ::MiniKit::Graphics::CommandBuffer& commandBuffer) noexcept;
    void DrawField(::MiniKit::Engine::Context& context, ::MiniKit::Graphics::DrawInfo& drawSurface, ::MiniKit::Graphics::CommandBuffer& commandBuffer) noexcept;
    void DrawBlocks(::MiniKit::Engine::Context& context, ::MiniKit::Graphics::DrawInfo& drawSurface, ::MiniKit::Graphics::CommandBuffer& commandBuffer) noexcept;
    void DrawScore(::MiniKit::Engine::Context& context, ::MiniKit::Graphics::DrawInfo& drawSurface, ::MiniKit::Graphics::CommandBuffer& commandBuffer) noexcept;
    void DrawNumber(::MiniKit::Engine::Context& context, ::MiniKit::Graphics::DrawInfo& drawSurface,
                      ::MiniKit::Graphics::CommandBuffer& commandBuffer, int number) noexcept;
    void DrawLogo(::MiniKit::Engine::Context& context, ::MiniKit::Graphics::DrawInfo& drawSurface, ::MiniKit::Graphics::CommandBuffer& commandBuffer) noexcept;
    void ChangeState(States state, std::optional<bool> quit = false) noexcept;
    bool CheckCollision(Tetromino* tetromino);
    bool CheckSideCollision(int step);
    void MoveSide(int step);
    void GetGhostPosition();

    void UpdateSettings();
public:
    Game(::std::shared_ptr<App> app);

    virtual void Init(::MiniKit::Engine::Context& context) override;

    virtual void Tick(::MiniKit::Engine::Context& context, ::MiniKit::Graphics::DrawInfo& drawSurface, ::MiniKit::Graphics::CommandBuffer& commandBuffer) noexcept override;

    virtual void KeyDown(const ::MiniKit::Platform::KeyEvent& event) noexcept override;
    
    virtual void KeyUp(const ::MiniKit::Platform::KeyEvent& event) noexcept override;
};