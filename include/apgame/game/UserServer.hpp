#pragma once

#include <apgame/game/User.hpp>
#include <apgame/game/UserManager.hpp>
#include <apgame/game/GameContext.hpp>
#include <apgame/game/UserCommand.hpp>
#include <apgame/socket/SocketContext.hpp>

#include <mutex>
#include <string>
#include <unordered_map>

namespace apgame {

struct UserServer {

  UserServer (UserManager & user_manager, GameContext & game_context)
  : user_manager_(user_manager)
  , game_context_(game_context)
  , socket_context_(game_context.socket_context) {
  }

  void operator() () {
    LOG_INFO("start UserServer");
    while (spin()) {
    }
    LOG_INFO("end UserServer");
  }

private:
  UserManager & user_manager_;
  GameContext & game_context_;
  SocketContext & socket_context_;

  bool spin () {
    UserCommand cmd;
    if (!socket_context_.receive(cmd)) {
      LOG_ERROR("failed to receive command");
      return false;
    }

    switch (cmd) {
    case USER_COMMAND_JOIN_USER: return joinUser();
    case USER_COMMAND_EXIT: return exit();
    default:
      LOG_ERROR("unknown command:", cmd);
      return false;
    }
  }

/**
 * receive:
 * [std::string name]
 *
 * send:
 * [int error]
 *
 * error == -2: failed
 * error == -1: communication error
 * error == 0: success
 */
  bool joinUser () {
    LOG_DEBUG("joinUser");
    std::string name;
    int error;

    if (!socket_context_.receive(name, 128)) {
      LOG_ERROR("failed to receive name");
      return false;
    }
    LOG_DEBUG("name = ", name, ", name.size = ", name.size());
    game_context_.user = user_manager_.createUser(name);
    if (game_context_.user == nullptr) {
      game_context_.user = user_manager_.findUserByName(name);
    }

    if (game_context_.user != nullptr) {
      error = 0;
    } else {
      LOG_ERROR("failed to join game ");
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
    LOG_DEBUG("exit");
    int error = 0;
    if (!socket_context_.send(error)) {
      LOG_ERROR("failed to send error");
      return false;
    }
    return false;
  }
};

}
