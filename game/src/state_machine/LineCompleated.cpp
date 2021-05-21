#include <LineCompleated.h>
#include <Game.h>

using ::MiniKit::Platform::Keycode;

void LineCompleatedState::Tick(::MiniKit::Engine::Context& context, ::MiniKit::Graphics::DrawInfo& drawSurface, ::MiniKit::Graphics::CommandBuffer& commandBuffer) noexcept
{   
    auto game = m_Game.lock();
    
    if (!game) {
        return;
    }

    auto delta = context.GetFrameDelta();
    m_Value += delta;
    if (m_Value >= g_LineCompleatedAnimationTime) {
        game->m_ScoreManager->AddtoScore();
        game->m_GridManager->ClearLines();
        game->ChangeState(States::SPAWN);
    }
    for (int line : game->m_GridManager->GetCompleatedLines()) {
        for (int x = 0; x < g_FieldWidth; x++) {
            if (game->m_Blocks[line][x]) {
                GetColor(game->m_Blocks[line][x]->color, delta);
                game->m_Blocks[line][x]->color.alpha = GetAlpha();
            }
        }
    }
    
    GameState::Tick(context, drawSurface, commandBuffer);
}

void LineCompleatedState::GetColor(::MiniKit::Graphics::Color& color, float delta) noexcept
{
    float res = 1.0f - m_Value * (1.0f / g_LineCompleatedAnimationTime);

    if (res <= 0) {
        color = g_LineCompleatedColor;
    }
    else {
        color.red = color.red + ((g_LineCompleatedColor.red - color.red) / (g_LineCompleatedAnimationTime - m_Value)) * (delta);
        color.blue = color.blue + ((g_LineCompleatedColor.blue - color.blue) / (g_LineCompleatedAnimationTime - m_Value)) * (delta);
        color.green = color.green + ((g_LineCompleatedColor.green - color.green) / (g_LineCompleatedAnimationTime - m_Value)) * (delta);
    }
}

void LineCompleatedState::Enter() noexcept
{   
    m_Value = 0.0f;
}

LineCompleatedState::LineCompleatedState(std::shared_ptr<Game> game) :
            GameState(game)
{

}

LineCompleatedState::~LineCompleatedState()
{
    
}

float LineCompleatedState::GetAlpha() noexcept
{
    float res = g_LineCompleatedColor.alpha - m_Value * (g_LineCompleatedColor.alpha / g_LineCompleatedAnimationTime);

    if (res <= 0) {
        return 0;
    }

    return res;
}
