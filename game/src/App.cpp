#include <App.h>
#include <Game.h>
#include <Menu.h>
#include <Options.h>
#include <Help.h>


::std::error_code App::Start(::MiniKit::Engine::Context& context) noexcept
{   
    //window
    auto& window = context.GetWindow();
    window.AddResponder(*this);

    //images
    auto& graphicsDevice = context.GetGraphicsDevice();

    m_Images["block"] = graphicsDevice.CreateImage(g_BlockPath);
    m_Images["field"] = graphicsDevice.CreateImage(g_FieldPath);
    m_Images["back"] = graphicsDevice.CreateImage(g_BackPath);
    m_Images["border"] = graphicsDevice.CreateImage(g_BorderPath);
    for (int i = 0; i < 10; i++) {
        m_Images[std::to_string(i)] = graphicsDevice.CreateImage("assets/" + std::to_string(i) + ".png");
    }
    for (int i = 65; i <= 90; i++) {
        std::string letter {static_cast<char> (i)};

        m_Images[letter] = graphicsDevice.CreateImage("assets/" + letter + ".png");
    }
    m_Images[" "] = graphicsDevice.CreateImage(g_BlankPath);

    //background
    const auto drawableWidth = static_cast<float>(window.GetDrawableWidth());
    const auto drawableHeight = static_cast<float>(window.GetDrawableHeight());

    const auto& backSize = m_Images["back"]->GetSize();
    const auto backImageScaleY = drawableHeight / static_cast<float>(backSize.height);
    const auto backImageScaleX = drawableWidth / static_cast<float>(backSize.width);
    m_BackgroundScale = {backImageScaleX, backImageScaleY};

    //elements
    m_Elements[Element::GAME] = std::make_shared<Game> (shared_from_this());
    m_Elements[Element::MENU] = std::make_shared<Menu> (shared_from_this());
    m_Elements[Element::OPTIONS] = std::make_shared<Options> (shared_from_this());
    m_Elements[Element::HELP] = std::make_shared<Help> (shared_from_this());
    for (auto& [key, value] : m_Elements) {
        value->Init(context);
    }

    ChangeElement(Element::MENU);
    return {};
}


void App::Tick(::MiniKit::Engine::Context& context) noexcept
{   
    auto& graphicsDevice = context.GetGraphicsDevice();
    auto& commandBuffer = graphicsDevice.BeginFrame(1.0f, 1.0f, 1.0f, 1.0f);
    ::MiniKit::Graphics::DrawInfo drawSurface{};

    DrawBackground(context, drawSurface, commandBuffer);
    m_Elements[m_Element]->Tick(context, drawSurface, commandBuffer);


    graphicsDevice.EndFrame(commandBuffer);
}

void App::KeyDown(::MiniKit::Platform::Window& window, const ::MiniKit::Platform::KeyEvent& event) noexcept
{
    using ::MiniKit::Platform::Keycode;

    switch (event.keycode)
    {
        case Keycode::KeyLeft:
        case Keycode::KeyRight:
        case Keycode::KeyDown:
        case Keycode::KeyUp:
        case Keycode::KeyZ:
        case Keycode::KeyC:
        case Keycode::KeySpace:
        case Keycode::KeyEnter:
        {
            if (!m_KeyState[event.keycode])
            {
                m_KeyState[event.keycode] = true;
                m_Elements[m_Element]->KeyDown(event);
            }
            break;
        }
        default:
            break;
    }
}

void App::KeyUp(::MiniKit::Platform::Window& window, const ::MiniKit::Platform::KeyEvent& event) noexcept
{
    using ::MiniKit::Platform::Keycode;

    switch (event.keycode)
    {
        case Keycode::KeyLeft:
        case Keycode::KeyRight:
        case Keycode::KeyDown:
        case Keycode::KeyUp:
        case Keycode::KeyZ:
        case Keycode::KeyC:
        case Keycode::KeySpace:
        case Keycode::KeyEnter:
        {
            if (m_KeyState[event.keycode])
            {
                m_KeyState[event.keycode] = false;
                m_Elements[m_Element]->KeyUp(event);
            }
            break;
        }
        default:
            break;
    }
}

void App::DrawBackground(::MiniKit::Engine::Context& context, ::MiniKit::Graphics::DrawInfo& drawSurface,
                      ::MiniKit::Graphics::CommandBuffer& commandBuffer) noexcept
{
    commandBuffer.SetImage(*m_Images["back"]);

    drawSurface.tint = g_BackgroundColor;
    drawSurface.position.x = 0;
    drawSurface.position.y = 0;
    drawSurface.scale = m_BackgroundScale;

    commandBuffer.Draw(drawSurface);
}

void App::ChangeElement(Element element) noexcept
{   
    if (m_Element == element) {
        return;
    }
    if (m_Element != Element::COUNT) {
        m_Elements[m_Element]->Exit();
    }
    m_Element = element;
    m_Elements[m_Element]->Enter();
}

::std::error_code App::Shutdown(::MiniKit::Engine::Context& context) noexcept
{
    return {};
}

void App::StartNewGame() noexcept
{
    static_cast<Game*> (m_Elements[Element::GAME].get())->ChangeState(States::NEW_GAME);
    ChangeElement(Element::GAME);
}

const MiniKit::Graphics::Image& App::GetImage(std::string name) noexcept
{
    return *m_Images[name];
}

bool App::GetGameState() noexcept
{
    return static_cast<Game*> (m_Elements[Element::GAME].get())->m_Pause;
}

void App::SetGhostPiece(bool value) noexcept
{
    static_cast<Game*> (m_Elements[Element::GAME].get())->m_Ghost = value;
}

void App::DrawText(::MiniKit::Graphics::DrawInfo& drawSurface,
                    ::MiniKit::Graphics::CommandBuffer& commandBuffer, const ::MiniKit::Graphics::Color& color,
                    const std::string text, float& x, float& y, float width, float height) noexcept
{
    for (const char c : text) {
        if (!iswalnum(c) && !isspace(c)) {
            continue;
        }
        char C = std::toupper(c);
        auto image = m_Images[std::string {C}];

        drawSurface.position.y = y;
        drawSurface.position.x = x;
        drawSurface.tint = color;
        drawSurface.scale = {width / image->GetSize().width, height / image->GetSize().height};

        commandBuffer.SetImage(*image.get());
        commandBuffer.Draw(drawSurface);

        x += width;
    }
}