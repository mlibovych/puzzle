#pragma once

#include <array>
#include <chrono>
#include <deque>
#include <iostream>
#include <memory>
#include <vector>
#include <unordered_map>

#include <MiniKit/MiniKit.hpp>

class App;

class AppElement
{
    friend class App;
protected:
    std::weak_ptr<App> m_App;
public:
    AppElement(std::shared_ptr<App> app);

    virtual ~AppElement();

    virtual void Init(::MiniKit::Engine::Context& context) = 0;

    virtual void Tick(::MiniKit::Engine::Context& context, ::MiniKit::Graphics::DrawInfo& drawSurface, ::MiniKit::Graphics::CommandBuffer& commandBuffer) noexcept = 0;

    virtual void KeyDown(const ::MiniKit::Platform::KeyEvent& event) noexcept;
    
    virtual void KeyUp(const ::MiniKit::Platform::KeyEvent& event) noexcept;

    virtual void Enter() noexcept;

    virtual void Exit() noexcept;
};