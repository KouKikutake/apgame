#pragma once

#include <apgame/game/enum.hpp>
#include <apgame/game/User.hpp>
#include <apgame/game/UserManager.hpp>
#include <apgame/game/GameContext.hpp>
#include <apgame/game/RoomManager.hpp>
#include <apgame/game/RoomCommand.hpp>

#include <apgame/core/logging.hpp>

#include <unordered_map>
#include <string>

namespace apgame {

struct RoomServer {

  RoomServer (RoomManager & room_manager, GameContext & game_context)
  : room_manager_(room_manager)
  , game_context_(game_context)
  , socket_context_(game_context.socket_context) {
  }

  void operator() () {
    LOG_INFO("start RoomServer");

    while (spin()) {}

    LOG_INFO("end RoomServer");
  }

private:

  RoomManager & room_manager_;
  GameContext & game_context_;
  SocketContext & socket_context_;

  bool spin () {
    RoomCommand cmd;
    if (!socket_context_.receive(cmd)) {
      LOG_ERROR("failed to receive command");
      return false;
    }

    switch (cmd) {
    case ROOM_COMMAND_JOIN_ROOM: return joinRoom();
    case ROOM_COMMAND_EXIT: return exit();
    default:
      LOG_ERROR("unknown command:", cmd);
      return false;
    }
  }

/**
 *  receive:
 *  [std::string room_name][std::string game_name]
 *
 *  send:
 *  [int error]
 *
 *  error == -3: user is not specified
 *  error == -2: failed to join room
 *  error == -1: communication error
 *  error == 0: success
 */
  bool joinRoom () {
    std::string room_name;
    std::string game_name;
    int error;

    if (!socket_context_.receive(room_name, 128)) {
      LOG_ERROR("failed to receive room name");
      return false;
    }
    if (!socket_context_.receive(game_name, 128)) {
      LOG_ERROR("failed to receive game name");
      return false;
    }

    if (!game_context_.user) {
      LOG_ERROR("user is not specified. forget to pass UserServer?");
      if (!socket_context_.send(-3)) {
        LOG_ERROR("failed to send error");
        return false;
      }
      return true;
    }

    error = 0;
    game_context_.room = room_manager_.createRoom(*game_context_.user, room_name, game_name);
    if (game_context_.room == nullptr) {
      LOG_ERROR("failed to create room");
      game_context_.room = room_manager_.joinRoom(*game_context_.user, room_name, game_name);
    }

    if (game_context_.room == nullptr) {
      LOG_ERROR("failed to join room");
      error = -2;
    }

    if (!socket_context_.send(error)) {
      LOG_ERROR("failed to send error");
      return false;
    }
    return true;
  }

/**
 *  receive:
 *
 *  send:
 *  [int error]
 *
 *  error == -1: communication error
 *  error == 0: success
 */
  bool exit () {
    int error = 0;
    if (!socket_context_.send(error)) {
      LOG_ERROR("failed to send error");
      return false;
    }
    return false;
  }

};

}
