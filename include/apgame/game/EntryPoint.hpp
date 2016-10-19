#pragma once

#include <apgame/core/logging.hpp>
#include <apgame/socket/SocketContext.hpp>
#include <apgame/game/GameContext.hpp>
#include <apgame/game/RoomServer.hpp>
#include <apgame/game/UserServer.hpp>

namespace apgame {

struct EntryPoint {

  void operator() (SocketContext & socket_context) {
    LOG_INFO("entry point enter");
    GameContext game_context(socket_context);

    LOG_INFO("user server");
    UserServer user_server(user_manager_, game_context);
    user_server();

    LOG_INFO("room server");
    RoomServer room_server(room_manager_, game_context);
    room_server();

    if (game_context.user == nullptr || game_context.room == nullptr) {
      LOG_ERROR("failed to start game");
      return;
    }

    game_context.game = game_context.room->getGame();

    int num_round = game_context.room->getNumRound();

    LOG_INFO("start round");
    for (int i = 0; i < num_round; ++i) {
      if (game_context.game->initialize()) {
        LOG_INFO("round ", i);
        LOG_INFO("initialized!");
      }
      game_context.game->run(game_context);
    }

    LOG_INFO("entry point exit");
  }

private:
  UserManager user_manager_;
  RoomManager room_manager_;
};

}
