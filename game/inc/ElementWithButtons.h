#pragma once

#include <array>
#include <chrono>
#include <deque>
#include <iostream>
#include <memory>
#include <vector>
#include <variant>
#include <unordered_map>

#include <App.h>

#include <MiniKit/MiniKit.hpp>

class App;

struct Button
{
    std::string title { "Button" };
    bool active { true };
    std::function<void()> callback;
};

struct Option
{
    std::string title { "Option" };
    std::deque<Button> m_Buttons { };
    int m_ActiveButtonIdx { 0 };

    template <typename T>
    void AddVariant(T&& button) noexcept
    {
        m_Buttons.emplace_back(button);
    }
};

class ElementWithButtons
{
    std::weak_ptr<App> m_App;
protected:
    std::deque<std::variant<Button, Option>> m_Elements { };
    int m_ActiveElementIdx { 0 };
    void GetNextButton() noexcept;
    void GetPreviousButton() noexcept;
    void DrawElementsVertical(::MiniKit::Graphics::DrawInfo& drawSurface, ::MiniKit::Graphics::CommandBuffer& commandBuffer, float x, float y, float width, float height) noexcept;
    void DrawOption(const Option& option, bool active, const ::MiniKit::Graphics::Color& color, ::MiniKit::Graphics::DrawInfo& drawSurface, ::MiniKit::Graphics::CommandBuffer& commandBuffer, float x, float y, float height) noexcept;
    void DrawButton(const Button& button, bool active, const ::MiniKit::Graphics::Color& color, ::MiniKit::Graphics::DrawInfo& drawSurface, ::MiniKit::Graphics::CommandBuffer& commandBuffer, float x, float y, float width, float height) noexcept;
public:
    ElementWithButtons(std::shared_ptr<App> app) : m_App(app)
    {

    }
    virtual void KeyDown(const ::MiniKit::Platform::KeyEvent& event) noexcept;
};