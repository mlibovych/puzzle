#pragma once

#include <array>
#include <chrono>
#include <iostream>
#include <memory>
#include <vector>
#include <unordered_map>

#include <StateMachine.h>

#include <MiniKit/MiniKit.hpp>

constexpr int g_BlockWidth = 80;
constexpr int g_AppWidth = 800;
constexpr int g_AppHeight = 900;
constexpr int g_Padding = 10;

const std::string g_BlockPath = "assets/bl.png"; 
const std::string g_BackPath = "assets/3.png";

class StateMachine;

class Game final : public ::MiniKit::Engine::Application, public ::MiniKit::Platform::Responder,
                   public std::enable_shared_from_this<Game>
{
    StateMachine* m_State;
    ::std::unordered_map<States, ::std::unique_ptr<StateMachine>> m_States;

    
public:
    ::std::error_code Start(::MiniKit::Engine::Context& context) noexcept override;
    
    ::std::error_code Shutdown(::MiniKit::Engine::Context& context) noexcept override;
    
    void Tick(::MiniKit::Engine::Context& context) noexcept override;
};