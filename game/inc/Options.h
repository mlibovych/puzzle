#pragma once

#include <array>
#include <chrono>
#include <deque>
#include <iostream>
#include <memory>
#include <vector>
#include <unordered_map>

#include <App.h>
#include <AppElement.h>
#include <ElementWithButtons.h>

#include <MiniKit/MiniKit.hpp>

class Options : public AppElement , public ElementWithButtons
{
public:
    Options(::std::shared_ptr<App> app);

    virtual void Init(::MiniKit::Engine::Context& context) override;

    virtual void Exit() noexcept override;

    virtual void Tick(::MiniKit::Engine::Context& context, ::MiniKit::Graphics::DrawInfo& drawSurface, ::MiniKit::Graphics::CommandBuffer& commandBuffer) noexcept override;

    virtual void KeyDown(const ::MiniKit::Platform::KeyEvent& event) noexcept override;
};