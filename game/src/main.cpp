#include <iostream>

#include <Game.h>

int main(int argc, char** argv)
{
    // Settings settings;    
    // settings.ParseFile();
    // decltype(auto) name = settings.GetData().GetString("max");
    // FormatObject shape = settings.GetData().GetObject("shape");
    // decltype(auto) rows = shape.GetInt("rows");

    // for (int i = 0; i < rows; i++) {
    //     decltype(auto) row = shape.GetVector(std::to_string(i));
    //     for (auto el : row) {
    //         std::cout << std::get<int> (el) << " ";
    //     }
    //     std::cout << std::endl;
    // }

    auto game = std::make_shared<Game> ();
 
    ::MiniKit::Engine::StartupInfo startupInfo{};
    
    startupInfo.window.title = "Puzzle";
    startupInfo.window.width = g_AppWidth;
    startupInfo.window.height = g_AppHeight;
    
    return ::MiniKit::ApplicationMain(startupInfo, *game);
}
