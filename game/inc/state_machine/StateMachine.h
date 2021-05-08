#pragma once

#include <memory>

class Game;

class StateMachine {
    std::weak_ptr<Game> m_game;
public:
};