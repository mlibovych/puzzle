#include <LeaderBoard.h>

LeaderBoard::LeaderBoard(::std::shared_ptr<App> app) : AppElement(app), ElementWithButtons(app)
{

}

void LeaderBoard::Update() noexcept
{
    m_Ok = true;

    std::ifstream file(g_PathToLeaderBoard);
    std::string line;

    try {
        if (!file.is_open()) {
            throw std::invalid_argument(g_PathToLeaderBoard);
        }
        int count = 0;

        while(getline(file, line)) {
            m_Table.insert(std::stoi(line));
            if (++count >= 10) {
                break;
            }
        }
    }
    catch (std::exception& ex) {
        m_Ok = false;
    }
    if (file.is_open()) {
        file.close();
    }
}

void LeaderBoard::Init(::MiniKit::Engine::Context& context)
{   
    Update();

    m_Elements.push_back(Button{"Back", true, [&]() {
        auto app = AppElement::m_App.lock();

        app->ChangeElement(Element::MENU);
    }});
}

void LeaderBoard::Tick(::MiniKit::Engine::Context& context, ::MiniKit::Graphics::DrawInfo& drawSurface, ::MiniKit::Graphics::CommandBuffer& commandBuffer) noexcept
{
    auto app = AppElement::m_App.lock();

    float startY = 1800.0f / 2 - g_Padding * 2 - 50.0f / 2;
    float startX = 0;

    if (m_Ok) {
        startX = 0 - (m_Title.size() - 1) * 60.0f / 2;
        app->DrawText(drawSurface, commandBuffer, g_OrangeColor, m_Title, startX, startY, 60, 70);
        startY -= 70 + g_Padding * 3;

        for (auto it = m_Table.rbegin(); it != m_Table.rend(); it++) {
            auto record = std::to_string(*it);

            startX = 0 - (record.size() - 1) * 40.0f / 2;
            app->DrawText(drawSurface, commandBuffer, g_WhiteColor, std::to_string(*it), startX, startY, 40, 50);
            startY -= 50 + g_Padding * 3;   
        }
    }
    else {
        startX = 0 - (m_WarningLabel.size() - 1) * 40.0f / 2;
        app->DrawText(drawSurface, commandBuffer, g_OrangeColor, m_WarningLabel, startX, startY, 40, 50);
        startY -= 50 + g_Padding * 3;
    }

    startX = 0;
    DrawElementsVertical(drawSurface, commandBuffer, 0, startY - 50 / 2, 400, 100);
}

void LeaderBoard::KeyDown(const ::MiniKit::Platform::KeyEvent& event) noexcept
{
    ElementWithButtons::KeyDown(event);
}

void LeaderBoard::Add(int result) noexcept
{
    if (result > *m_Table.begin()) {
        m_Table.insert(result);
    }

    std::ofstream file(g_PathToLeaderBoard);

    if (!file.is_open()) {
        m_Ok = false;
        return;
    }
    for (auto it = m_Table.rbegin(); it != m_Table.rend(); it++) {
        auto record = std::to_string(*it);

        file << record << "\n";
    }
}