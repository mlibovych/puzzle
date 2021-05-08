#pragma once

#include <array>
#include <chrono>
#include <iostream>
#include <memory>
#include <vector>
#include <unordered_map>

#include <StateMachine.h>
#include <Tetromino.h>

#include <MiniKit/MiniKit.hpp>

constexpr int g_BlockWidth = 80;
constexpr int g_AppWidth = 800;
constexpr int g_AppHeight = 900;
constexpr int g_Padding = 10;

constexpr int g_FieldWidth = 10;
constexpr int g_FieldHeight = 20;

const std::string g_BlockPath = "assets/bl.png"; 
const std::string g_BackPath = "assets/2.png"; 

struct alignas(16) SpriteEntity
{
    ::MiniKit::Graphics::Color color{ 1.0f, 1.0f, 1.0f, 1.0f };
    ::MiniKit::Graphics::float2 position{ 0.0f, 0.0f };
    ::MiniKit::Graphics::float2 scale{ 1.0f, 1.0f };

    ::std::string imageName;
    // ::std::shared_ptr<::MiniKit::Graphics::Image> image{ nullptr };
};

class StateMachine;

class Game final : public ::MiniKit::Engine::Application, public ::MiniKit::Platform::Responder
{
    ::std::unique_ptr<StateMachine> m_State;

    ::std::unordered_map<::std::string, ::std::shared_ptr<::MiniKit::Graphics::Image>> m_Images;
    // ::std::unique_ptr<Tetromino> m_Tetromino{ nullptr };

    ::std::array<::std::array<int, 10>, 20> m_Field;
    ::std::array<::std::array<SpriteEntity, 10>, 20> m_Background;
    ::std::vector<SpriteEntity> m_Blocks;

    uint64_t m_FrameTime;
public:
    ::std::error_code Start(::MiniKit::Engine::Context& context) noexcept override;
    
    ::std::error_code Shutdown(::MiniKit::Engine::Context& context) noexcept override;
    
    void Tick(::MiniKit::Engine::Context& context) noexcept override;
};