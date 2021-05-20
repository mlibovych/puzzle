#pragma once

#include <array>
#include <chrono>
#include <deque>
#include <iostream>
#include <memory>
#include <vector>
#include <unordered_map>

#include <MiniKit/MiniKit.hpp>
#include <imgui.h>
#include <AppElement.h>

constexpr int g_FieldWidth = 10;
constexpr int g_FieldHeight = 20;

constexpr int g_BlockWidth = 80;
constexpr int g_AppWidth = 800;
constexpr int g_AppHeight = 900;
constexpr int g_Padding = 10;

constexpr float g_FallSpeed = 0.600f;

constexpr float g_LineCompleatedAnimationTime = 0.5f;
const ::MiniKit::Graphics::Color g_LineCompleatedColor{ 0.9f, 0.4f, 0.1f, 1.0f };
const ::MiniKit::Graphics::Color g_OrangeColor { 1.0f, 0.38f, 0.05f, 1.0f };
const ::MiniKit::Graphics::Color g_BackgroundColor { 0.1f, 0.1f, 0.1f, 1.0f };
const ::MiniKit::Graphics::Color g_GhostColor { 1.0f, 0.55f, 0.22f, 0.5f };
const ::MiniKit::Graphics::Color g_WhiteColor { 1.0f, 1.0f, 1.0f, 1.0f };
const ::MiniKit::Graphics::Color g_BlackColor { 0.0f, 0.0f, 0.0f, 1.0f };

const std::string g_BlockPath = "assets/sq.png"; 
const std::string g_FieldPath = "assets/back.png";
const std::string g_BackPath = "assets/pure.png";
const std::string g_BorderPath = "assets/border.png";
const std::string g_BlankPath = "assets/blank.png";

enum class Element
{
    MENU,
    GAME,
    OPTIONS,
    HELP,
    LEADERBOARD,
    COUNT
};

class App final : public ::MiniKit::Engine::Application, public ::MiniKit::Platform::Responder,
                   public std::enable_shared_from_this<App> 
{
    friend class Menu;
    friend class Game;
    friend class Option;
    friend class Help;
    friend class LeaderBoard;

    Element m_Element { Element::COUNT };
    ::std::unordered_map<Element, ::std::shared_ptr<AppElement>> m_Elements { };

    ::std::unordered_map<::std::string, ::std::shared_ptr<::MiniKit::Graphics::Image>> m_Images;
    ::MiniKit::Graphics::float2 m_BackgroundScale{ 1.0f, 1.0f };

    std::unordered_map<MiniKit::Platform::Keycode, bool> m_KeyState {};
    
    void DrawBackground(::MiniKit::Engine::Context& context, ::MiniKit::Graphics::DrawInfo& drawSurface,::MiniKit::Graphics::CommandBuffer& commandBuffer) noexcept;
public:
    ::std::error_code Start(::MiniKit::Engine::Context& context) noexcept override;
    
    ::std::error_code Shutdown(::MiniKit::Engine::Context& context) noexcept override;
    
    void Tick(::MiniKit::Engine::Context& context) noexcept override;

    virtual void KeyDown(::MiniKit::Platform::Window& window, const ::MiniKit::Platform::KeyEvent& event) noexcept override;
    
    virtual void KeyUp(::MiniKit::Platform::Window& window, const ::MiniKit::Platform::KeyEvent& event) noexcept override;

    void ChangeElement(Element element) noexcept;

    void DrawText(::MiniKit::Graphics::DrawInfo& drawSurface,
                    ::MiniKit::Graphics::CommandBuffer& commandBuffer, const ::MiniKit::Graphics::Color& color,
                    const std::string text, float& x, float& y, float width, float height) noexcept;

    void StartNewGame() noexcept;

    const MiniKit::Graphics::Image& GetImage(std::string name) noexcept;

    bool GetGameState() noexcept;

    void SetGhostPiece(bool value) noexcept;

    void SaveResult(int result) noexcept;
};