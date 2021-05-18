#include <iostream>

#include <Game.h>

int main(int argc, char** argv)
{   
    auto app = std::make_shared<App> ();
 
    ::MiniKit::Engine::StartupInfo startupInfo{};
    
    startupInfo.window.title = "Puzzle";
    startupInfo.window.width = g_AppWidth;
    startupInfo.window.height = g_AppHeight;
    
    return ::MiniKit::ApplicationMain(startupInfo, *app);
}
