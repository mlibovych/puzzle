#pragma once

#include <array>
#include <chrono>
#include <deque>
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>
#include <set>
#include <unordered_map>

#include <App.h>
#include <AppElement.h>
#include <ElementWithButtons.h>

#include <MiniKit/MiniKit.hpp>

const std::string g_PathToLeaderBoard = "game/.leaderboard.txt";

class LeaderBoard : public AppElement , public ElementWithButtons
{
    std::multiset<int> m_Table { };
    bool m_Ok { true };
    std::string m_WarningLabel { "Somebody corrupted your saves" };
    std::string m_Title { "Top 10 results" };
public:
    LeaderBoard(::std::shared_ptr<App> app);

    virtual void Init(::MiniKit::Engine::Context& context) override;

    virtual void Tick(::MiniKit::Engine::Context& context, ::MiniKit::Graphics::DrawInfo& drawSurface, ::MiniKit::Graphics::CommandBuffer& commandBuffer) noexcept override;

    virtual void KeyDown(const ::MiniKit::Platform::KeyEvent& event) noexcept override;

    void Update() noexcept;

    void Add(int result) noexcept;
};