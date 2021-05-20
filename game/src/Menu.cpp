#include <Menu.h>

Menu::Menu(::std::shared_ptr<App> app) : AppElement(app), ElementWithButtons(app)
{
    
}

void Menu::Init(::MiniKit::Engine::Context& context)
{
    m_Elements.push_back(Button{"New game", true, [&]() {
        auto app = AppElement::m_App.lock();

        app->StartNewGame();
    }});
    m_Elements.push_back(Button{"Resume", true, [&]() {
        auto app = AppElement::m_App.lock();

        app->ChangeElement(Element::GAME);
    }});
    m_Elements.push_back(Button{"Options", true, [&]() {
        auto app = AppElement::m_App.lock();

        app->ChangeElement(Element::OPTIONS);
    }});
    m_Elements.push_back(Button{"Leaders", true, [&]() {
        auto app = AppElement::m_App.lock();

        app->ChangeElement(Element::LEADERBOARD);
    }});
    m_Elements.push_back(Button{"Help", true, [&]() {
        auto app = AppElement::m_App.lock();

        app->ChangeElement(Element::HELP);
    }});
    m_Elements.push_back(Button{"Exit", true , [&] () {
        context.Terminate();
    }});
}

void Menu::Enter() noexcept
{
    auto resumeButtonIt = std::find_if(m_Elements.begin(), m_Elements.end(), [] (const auto& b) {
        return std::get<Button> (b).title == "Resume";
    });
    std::get<Button>(*resumeButtonIt).active = AppElement::m_App.lock()->GetGameState();
}

void Menu::Tick(::MiniKit::Engine::Context& context, ::MiniKit::Graphics::DrawInfo& drawSurface, ::MiniKit::Graphics::CommandBuffer& commandBuffer) noexcept
{   
    float startX = 0;
    float startY = 1800.0f / 2 - g_Padding * 2 - 400;

    DrawElementsVertical(drawSurface, commandBuffer, startX, startY, 400, 100);
}

void Menu::KeyDown(const ::MiniKit::Platform::KeyEvent& event) noexcept
{
    ElementWithButtons::KeyDown(event);
}