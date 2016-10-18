#pragma once

#include <apgame/game/Game.hpp>
#include <apgame/reversi/Reversi.hpp>

#include <string>

namespace apgame {

std::unique_ptr<Game> buildGame (std::string const & game_name) {
  LOG_DEBUG("build game");
  if (game_name == "Reversi") {
    return std::unique_ptr<Game>(new Reversi);
  }
  return std::unique_ptr<Game>(nullptr);
}

}
