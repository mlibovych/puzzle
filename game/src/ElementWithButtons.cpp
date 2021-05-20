#include <ElementWithButtons.h>

using ::MiniKit::Platform::Keycode;

void ElementWithButtons::DrawOption(const Option& option, bool active, const ::MiniKit::Graphics::Color& color, ::MiniKit::Graphics::DrawInfo& drawSurface, ::MiniKit::Graphics::CommandBuffer& commandBuffer, float x, float y, float height) noexcept
{   
    auto app = m_App.lock();
    auto& image = app->GetImage("border");
    auto textX = x - ((option.title.size() - 1) * 40 + option.m_Buttons.size() * 200) / 2;
    auto scaleX = (static_cast<float> (option.title.size()) * 40 + option.m_Buttons.size() * 200 + g_Padding * 2) / image.GetSize().width;
    auto scaleY = height / static_cast<float> (image.GetSize().height);
    auto buttonColor = g_OrangeColor;

    if (active) {
        buttonColor = g_BlackColor;
        commandBuffer.SetImage(image);
        drawSurface.tint = color;
        drawSurface.position = { x, y };
        drawSurface.scale = { scaleX, scaleY };

        commandBuffer.Draw(drawSurface);
    }
    auto textColor = g_WhiteColor;
    app->DrawText(drawSurface, commandBuffer, textColor,
        option.title, textX, y, 40, 50);

    int count = 0;
    auto startX = textX + 200 / 2  - 40 / 2;

    for (const auto& button : option.m_Buttons) {
        bool active = option.m_ActiveButtonIdx == count;

        DrawButton(button, active, buttonColor, drawSurface, commandBuffer, startX, y, 200, 80);
        startX += 200.0f;
        count++;
    }
}

void ElementWithButtons::DrawElementsVertical(::MiniKit::Graphics::DrawInfo& drawSurface, ::MiniKit::Graphics::CommandBuffer& commandBuffer, float x, float y, float width, float height) noexcept
{
    auto app = m_App.lock();
    auto& image = app->GetImage("border");
    
    
    size_t count = 0;
    for (const auto& element : m_Elements) {
        bool active  =  m_ActiveElementIdx == count;
        y -= height / 2;
        if (std::holds_alternative<Button> (element)) {
            const auto& button = std::get<Button>(element);

            DrawButton(button,active, g_OrangeColor, drawSurface, commandBuffer, x, y, width, height);
        }
        else {
            auto& option = std::get<Option>(element);

            DrawOption(option, active, g_OrangeColor, drawSurface, commandBuffer, x, y, height);
        }
        y -= height / 2 + g_Padding;
        count++;
    }
}

void ElementWithButtons::GetNextButton() noexcept
{
    if (m_Elements.size() > 0) {
        if (++m_ActiveElementIdx > m_Elements.size() - 1) {
            m_ActiveElementIdx = 0;
        }
        if (std::holds_alternative<Button> (m_Elements[m_ActiveElementIdx]) && !std::get<Button> (m_Elements[m_ActiveElementIdx]).active) {
            GetNextButton();
        }
    }
}

void ElementWithButtons::GetPreviousButton() noexcept
{   
    if (m_Elements.size() > 0) {
        if (--m_ActiveElementIdx < 0) {
            m_ActiveElementIdx = m_Elements.size() - 1;
        }
        if (std::holds_alternative<Button> (m_Elements[m_ActiveElementIdx]) && !std::get<Button> (m_Elements[m_ActiveElementIdx]).active) {
            GetPreviousButton();
        }
    }
}

void ElementWithButtons::DrawButton(const Button& button, bool active, const ::MiniKit::Graphics::Color& color, ::MiniKit::Graphics::DrawInfo& drawSurface, ::MiniKit::Graphics::CommandBuffer& commandBuffer, float x, float y, float width, float height) noexcept
{    
    auto app = m_App.lock();
    auto& image = app->GetImage("border");

    auto textX = x - (button.title.size() - 1 ) * 40 / 2;
    auto scaleX = width / image.GetSize().width;
    auto scaleY = height / static_cast<float> (image.GetSize().height);

    if (active) {
        commandBuffer.SetImage(image);
        drawSurface.tint = color;
        drawSurface.position = { x, y };
        drawSurface.scale = { scaleX, scaleY };

        commandBuffer.Draw(drawSurface);
    }
    auto textColor = g_WhiteColor;
    if (!button.active) {
        textColor.alpha = 0.5f;
    }
    app->DrawText(drawSurface, commandBuffer, textColor,
        button.title, textX, y, 40, 50);
}

void ElementWithButtons::KeyDown(const ::MiniKit::Platform::KeyEvent& event) noexcept
{
    switch (event.keycode)
    {
        case Keycode::KeyDown:
        {   
            GetNextButton();
            break;
        }
        case Keycode::KeyUp:
        {   
            GetPreviousButton();
            break;
        }
        case Keycode::KeyRight:
        {   
            if (std::holds_alternative<Option> (m_Elements[m_ActiveElementIdx])) {
                auto& option = std::get<Option> (m_Elements[m_ActiveElementIdx]);

                if (option.m_ActiveButtonIdx < option.m_Buttons.size() - 1) {
                    option.m_ActiveButtonIdx++;
                    option.m_Buttons[option.m_ActiveButtonIdx].callback();
                }
            }
            break;
        }
        case Keycode::KeyLeft:
        {   
            if (std::holds_alternative<Option> (m_Elements[m_ActiveElementIdx])) {
                auto& option = std::get<Option> (m_Elements[m_ActiveElementIdx]);

                if (option.m_ActiveButtonIdx > 0) {
                    option.m_ActiveButtonIdx--;
                    option.m_Buttons[option.m_ActiveButtonIdx].callback();
                }
            }
            break;
        }
        case Keycode::KeyEnter:
        {   
            if (m_Elements.size() > 0) {
                if (std::holds_alternative<Button> (m_Elements[m_ActiveElementIdx])) {
                    std::get<Button> (m_Elements[m_ActiveElementIdx]).callback();
                }
            }
        }
        default:
            break;
    }
}