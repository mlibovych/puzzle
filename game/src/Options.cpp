#include <Options.h>

Options::Options(::std::shared_ptr<App> app) : AppElement(app), ElementWithButtons(app)
{

}

void Options::Init(::MiniKit::Engine::Context& context)
{   
    Option option;
    option.title = "Ghost piece";
    option.AddVariant(Button{"On", true, [&]() {
        auto app = AppElement::m_App.lock();

        app->SetGhostPiece(true);
    }});
    option.AddVariant(Button{"Off", true, [&]() {
        auto app = AppElement::m_App.lock();

        app->SetGhostPiece(false);
    }});

    m_Elements.push_back(option);
    m_Elements.push_back(Button{"Back", true, [&]() {
        auto app = AppElement::m_App.lock();

        app->ChangeElement(Element::MENU);
    }});
}

void Options::Exit() noexcept
{
    m_ActiveElementIdx = 0;
}

void Options::Tick(::MiniKit::Engine::Context& context, ::MiniKit::Graphics::DrawInfo& drawSurface, ::MiniKit::Graphics::CommandBuffer& commandBuffer) noexcept
{
    auto app = AppElement::m_App.lock();

    float startX = 0;
    float startY = 1800.0f / 2 - g_Padding * 2 - 400;

    DrawElementsVertical(drawSurface, commandBuffer, startX, startY, 400, 100);
}

void Options::KeyDown(const ::MiniKit::Platform::KeyEvent& event) noexcept
{
    ElementWithButtons::KeyDown(event);
}
