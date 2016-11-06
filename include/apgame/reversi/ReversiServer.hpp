#pragma once

#include <apgame/reversi/Reversi.hpp>
#include <apgame/reversi/ReversiContext.hpp>
#include <apgame/socket/SocketContext.hpp>
#include <apgame/socket/Lock.hpp>

#include <array>
#include <atomic>
#include <ctime>
#include <mutex>
#include <unordered_map>
#include <random>

namespace apgame {

struct ReversiServer {

  ReversiServer (Reversi & reversi, SocketContext & socket_context, User & user)
  : reversi_(reversi)
  , socket_context_(socket_context)
  , user_(user) {
  }

  void operator() () {
    LOG_INFO("reversi server start");
    while (spin()) {}
    LOG_INFO("reversi server end");
  }


private:
  Reversi & reversi_;
  SocketContext & socket_context_;
  User & user_;

  bool spin () {
    LOG_DEBUG("spin");
    ReversiCommand cmd;
    if (!socket_context_.receive(cmd)) {
      LOG_DEBUG("fail to receive command");
      return false;
    }

    switch (cmd) {
    case REVERSI_COMMAND_GET_COLOR:
      return getColor();
    case REVERSI_COMMAND_GET_STATUS:
      return getStatus();
    case REVERSI_COMMAND_GET_BOARD:
      return getBoard();
    case REVERSI_COMMAND_PUT_STONE:
      return putStone();
    case REVERSI_COMMAND_GET_LAST_STONE:
      return getLastStone();
    default:
      LOG_DEBUG("unknown command ", cmd);
      return false;
    }
  }

/**
 * receive:
 *
 * send:
 * [ReversiStone color]
 */
  bool getColor () {
    LOG_DEBUG(user_.getName(), " : getColor");
    if (!socket_context_.send(reversi_.getColor(user_))) {
      LOG_ERROR("failed to send color");
      return false;
    }
    return true;
  }

/**
 * receive:
 *
 * send:
 * [ReversiStatus status]
 */
  bool getStatus () {
    LOG_DEBUG(user_.getName(), " : getStatus");
    if (!socket_context_.send(reversi_.getStatus(user_))) {
      LOG_ERROR("failed to send status");
      return false;
    }
    return true;
  }

/**
 *  receive:
 *
 *  send:
 *  [ReversiStone stone] * 64
 */
  bool getBoard () {
    LOG_DEBUG(user_.getName(), ": getBoard");
    std::array<ReversiStone, 64> board = reversi_.getBoard();
    if (!socket_context_.send(board)) {
      LOG_ERROR("failed to send board");
      return false;
    }
    return true;
  }

/**
 *  receive:
 *  [int x][int y]
 *
 *  send:
 *  [int error]
 *
  *  error = -5: you are not joined
 *  error = -3: invalid put
 *  error = -2: invalid turn
 *  error = -1: communication error
 *  error = 0: success
 */
  bool putStone () {
    LOG_DEBUG(user_.getName(), ": putStone");
    int x, y;
    if (!socket_context_.receive(x)) {
      LOG_ERROR("failed to receive x");
      return false;
    }
    if (!socket_context_.receive(y)) {
      LOG_ERROR("failed to receive y");
      return false;
    }
    int error = reversi_.putStone(user_, x, y);
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
 *  [int x][int y]
 *
*/
  bool getLastStone () {
    LOG_DEBUG(user_.getName(), " : getLastStone");
    std::pair<int, int> p = reversi_.getLastStone();
    if (!socket_context_.send(p.first)) {
      LOG_ERROR("failed to send x");
      return false;
    }
    if (!socket_context_.send(p.second)) {
      LOG_ERROR("failed to send y");
      return false;
    }
    return true;
  }

};

void reversi_run_proxy (Reversi & reversi, GameContext & game_context) {
  ReversiServer reversi_server(reversi, game_context.socket_context, *game_context.user);
  reversi_server();
}

}
