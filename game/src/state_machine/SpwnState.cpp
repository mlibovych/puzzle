#include <SpawnState.h>
#include <Game.h>

using ::MiniKit::Platform::Keycode;

SpawnState::SpawnState(std::shared_ptr<Game> game) :
            GameState(game)
{
    
}

SpawnState::~SpawnState()
{

}

void SpawnState::Tick(::MiniKit::Engine::Context& context, ::MiniKit::Graphics::DrawInfo& drawSurface, ::MiniKit::Graphics::CommandBuffer& commandBuffer) noexcept
{
    auto game = m_Game.lock();

    //events
    if (game->m_EventSystem->HaveEvents() &&
        game->m_EventSystem->GetNextEventType() == EventType::LINES_COMPLETED_EVENT) {
        game->ChangeState(States::LINE_COMPLEATED);
    }
    else {
        //tereomino
        std::mt19937 gen(m_Random());
        std::uniform_int_distribution<> dis(0, game->m_TetrominosFrequency - 1);
        int position = dis(gen);
        int sumFrequency = 0;

        game->m_EventSystem->ProccedEvent();
        game->m_Tetromino = ::std::make_unique<Tetromino> (*game->m_NextTetromino);

        for (auto& tetromino : game->m_Tetrominos) {
            sumFrequency +=tetromino->m_SpawnFrequency;
            if (sumFrequency >= position) {
                game->m_NextTetromino = ::std::make_unique<Tetromino> (*tetromino.get());
                break;
            }
        }
        int displacement = game->m_Tetromino->m_Shape.size() % 2 ? 1 : 0;

        game->m_Tetromino->m_X = g_FieldWidth / 2 - game->m_Tetromino->m_Shape.size() / 2 - displacement;
        game->m_Tetromino->m_Y = 0 - game->m_Tetromino->m_Shape.size() / 2;
        game->m_TetrominoGhost = ::std::make_unique<Tetromino> (*game->m_Tetromino.get());

        States state = States::POSITIONING;

        try {
            for (size_t y = 0; y < game->m_Tetromino->m_Shape.size(); y++) {
                for (size_t x = 0; x < game->m_Tetromino->m_Shape[y].size(); x++) {
                    if (game->m_Tetromino->m_Shape[y][x]) {
                        if (game->m_Tetromino->m_Y + static_cast<int> (y) >= 0 &&
                            game->m_Blocks[game->m_Tetromino->m_Y + y][game->m_Tetromino->m_X + x]) {
                            throw(true);
                        }
                    }
                }
            }
        }
        catch (bool) {
            state = States::GAME_OVER;
            game->m_Tetromino = nullptr;
        }
        game->ChangeState(state);
    }
    GameState::Tick(context, drawSurface, commandBuffer);
}

void SpawnState::Enter() noexcept
{   


}
