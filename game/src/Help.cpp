#include <Help.h>

Help::Help(::std::shared_ptr<App> app) : AppElement(app), ElementWithButtons(app)
{

}

void Help::Init(::MiniKit::Engine::Context& context)
{   
    m_Title = "Instructions";
    m_Text = {
        {{"Press ", 0}, {"arrowleft", 1}, {" or ", 0}, {"arrowright", 1}, {" to change position of the tetromino", 0}},
        {{"Hold ", 0}, {"arrowdown", 1}, {" to ", 0}, {"soft drop", 0}},
        {{"Press ", 0}, {"space", 1}, {" to ", 0}, {"hard drop", 0}},
        {{"Press ", 0}, {"arrowup", 1}, {" or key ", 0}, {"z", 1}, {" to rotate tetromino", 0}},
        {{"Press key ", 0}, {"c", 1}, {" to ", 0}, {"pause", 1}, {" the game and one more time to ", 0}, {"resume", 1}},
    };

    m_Elements.push_back(Button{"Back", true, [&]() {
        auto app = AppElement::m_App.lock();

        app->ChangeElement(Element::MENU);
    }});
}

void Help::Tick(::MiniKit::Engine::Context& context, ::MiniKit::Graphics::DrawInfo& drawSurface, ::MiniKit::Graphics::CommandBuffer& commandBuffer) noexcept
{
    auto app = AppElement::m_App.lock();

    float startY = 1800.0f / 2 - g_Padding * 2 - 50 / 2;
    float startX = 0 - (m_Title.size() - 1) * 40.0f / 2;

    app->DrawText(drawSurface, commandBuffer, g_WhiteColor, m_Title, startX, startY, 40, 50);
    startY -= 50 + g_Padding;

    for (const auto& sentence : m_Text) {
        startX = 1600.0f / -2 + 20 / 2 + g_Padding;
        for (const auto& part : sentence) {
            const auto& color = part.second ? g_OrangeColor : g_WhiteColor; 

            app->DrawText(drawSurface, commandBuffer, color, part.first, startX, startY, 20, 30);
        }
        startY -= 30 + g_Padding;
    }

    DrawElementsVertical(drawSurface, commandBuffer, 0, startY - 50 / 2, 400, 100);
}

void Help::KeyDown(const ::MiniKit::Platform::KeyEvent& event) noexcept
{
    ElementWithButtons::KeyDown(event);
}