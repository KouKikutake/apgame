#pragma once

#include <apgame/game/Game.hpp>
#include <apgame/game/enum.hpp>
#include <apgame/reversi/enum.hpp>
#include <apgame/reversi/ReversiContext.hpp>
#include <apgame/socket/Lock.hpp>

#include <array>
#include <atomic>
#include <ctime>
#include <mutex>
#include <unordered_map>
#include <random>

namespace apgame {

struct Reversi;

// implemented in ReversiServer.hpp
void reversi_run_proxy (Reversi & reversi, GameContext & game_context);

struct Reversi : public Game {

  Reversi ()
  : Game() {
    user_[0] = nullptr;
    user_[1] = nullptr;
    std::random_device device;
    random_engine_.seed(device());
    status_ = REVERSI_STATUS_BEFORE_GAME;
  }

  GameID gameID () const noexcept override {
    return GAME_ID_REVERSI;
  }

  char const * gameName () const noexcept override {
    return "Reversi";
  }

  std::size_t getMaxUser () const noexcept override {
    return 2;
  }

  bool join (User & user) override {
    std::lock_guard<std::mutex> lock(mtx_);
    if (user_[0] == &user || user_[0] == nullptr) {
      LOG_DEBUG(user.getName(), " joined!");
      user_[0] = &user;
      return true;
    }
    if (user_[1] == &user || user_[1] == nullptr) {
      LOG_DEBUG(user.getName(), " joined!");
      user_[1] = &user;
      return true;
    }
    return false;
  }

  bool initialize () override {
    std::lock_guard<std::mutex> lock(mtx_);
    if (!ready_()) {
      return false;
    }
    int i = std::uniform_int_distribution<int>(0, 1)(random_engine_);
    if (i == 0) {
      color_[0] = REVERSI_STONE_BLACK;
      color_[1] = REVERSI_STONE_WHITE;
    } else {
      color_[0] = REVERSI_STONE_WHITE;
      color_[1] = REVERSI_STONE_BLACK;
    }

    initBoard();
    last_passed_ = false;
    x_ = -1;
    y_ = -1;
    status_ = REVERSI_STATUS_BLACK_TURN;
    return true;
  }

  bool ready_ () {
    if (status_ != REVERSI_STATUS_BEFORE_GAME) {
      return false;
    }
    if (!user_[0] || !user_[1]) {
      return false;
    }
    return true;
  }

  void run (GameContext & game_context) override {
    reversi_run_proxy(*this, game_context);
  }

  ReversiStone getColor (User & user) const noexcept {
    std::lock_guard<std::mutex> lock(mtx_);
    return getColor_(user);
  }

  ReversiStone getColor_ (User & user) const noexcept {
    if (user_[0] == &user) {
      return color_[0];
    }
    if (user_[1] == &user) {
      return color_[1];
    }
    return REVERSI_STONE_EMPTY;
  }

  ReversiStatus getStatus () {
    std::lock_guard<std::mutex> lock(mtx_);
    return getStatus_();
  }

  ReversiStatus getStatus_ () {
    return status_;
  }

  std::array<ReversiStone, 64> getBoard () const {
    std::lock_guard<std::mutex> lock(mtx_);
    return getBoard_();
  }

  std::array<ReversiStone, 64> getBoard_ () const {
    return board_;
  }

/**
 *  @return
 *  error = -5: you are not joined
 *  error = -3: invalid put
 *  error = -2: invalid turn
 *  error = -1: communication error
 *  error = 0: success
 */
  int putStone (User & user, int x, int y) {
    std::lock_guard<std::mutex> lock(mtx_);
    ReversiStone color = getColor_(user);
    if (color == REVERSI_STONE_EMPTY) {
      return -5;
    } else if (color == REVERSI_STONE_BLACK && status_ != REVERSI_STATUS_BLACK_TURN) {
      return -2;
    } else if (color == REVERSI_STONE_WHITE && status_ != REVERSI_STATUS_WHITE_TURN) {
      return -2;
    }

    if (!checkPutStone(color, x, y, false)) {
      LOG_ERROR("invalid put");
      return -3;
    }

    x_ = x;
    y_ = y;

    status_ = (color == REVERSI_STONE_BLACK) ? REVERSI_STATUS_WHITE_TURN : REVERSI_STATUS_BLACK_TURN;
    return 0;
  }

  bool finishTurn (User & user) {
    std::lock_guard<std::mutex> lock(mtx_);
    return finishTurn_(user);
  }
  bool finishTurn_ (User & user) {
    ReversiStatus status = getStatus_();
    ReversiStone color = getColor_(user);
    if (color == REVERSI_STONE_EMPTY) {
      return false;
    } else if (color == REVERSI_STONE_BLACK && status != REVERSI_STATUS_BLACK_TURN) {
      return false;
    } else if (color == REVERSI_STONE_WHITE && status != REVERSI_STATUS_WHITE_TURN) {
      return false;
    }
    if (checkPass_(user)) {
      checkPutStone(color, x_, y_, true);
      last_passed_ = false;
    } else {
      last_passed_ = true;
    }

    x_ = -1;
    y_ = -1;

    if (status_ == REVERSI_STATUS_BLACK_TURN) {
      status_ = REVERSI_STATUS_WHITE_TURN;
    } else if (status_ == REVERSI_STATUS_WHITE_TURN) {
      status_ = REVERSI_STATUS_BLACK_TURN;
    }
  }

  std::pair<int, int> getLastStone () const noexcept {
    std::lock_guard<std::mutex> lock(mtx_);
    return std::pair<int, int>(x_, y_);
  }

  bool checkPass (User & user) {
    std::lock_guard<std::mutex> lock(mtx_);
    return checkPass_(user);
  }

  bool checkPass_ (User & user) {
    ReversiStatus status = getStatus_();
    ReversiStone color = getColor_(user);
    if (color == REVERSI_STONE_EMPTY) {
      return false;
    }
    if (color == REVERSI_STONE_BLACK && status != REVERSI_STATUS_BLACK_TURN) {
      return false;
    }
    if (color == REVERSI_STONE_WHITE && status != REVERSI_STATUS_WHITE_TURN) {
      return false;
    }
    return !checkPossibleChoice(color);
  }


private:

  mutable std::mutex mtx_;
  std::mt19937 random_engine_;

  std::array<User *, 2> user_;
  std::array<ReversiStone, 2> color_;
  std::array<ReversiStone, 64> board_;
  ReversiStatus status_;
  bool last_passed_;

  int x_;
  int y_;

  void initBoard () {
    for (ReversiStone & stone : board_) {
      stone = REVERSI_STONE_EMPTY;
    }

    board_[3 + 8 * 3] = REVERSI_STONE_WHITE;
    board_[4 + 8 * 4] = REVERSI_STONE_WHITE;
    board_[3 + 8 * 4] = REVERSI_STONE_BLACK;
    board_[4 + 8 * 3] = REVERSI_STONE_BLACK;

    x_ = -1;
    y_ = -1;
  }

  bool checkPossibleChoice (int color) {
    for (int x = 0; x < 8; ++x) {
      for (int y = 0; y < 8; ++y) {
        if (board_[x + 8 * y] != REVERSI_STONE_EMPTY) {
          continue;
        }
        if (checkPutStone(color, x, y, false)) {
          return true;
        }
      }
    }
    return false;
  }

  bool checkPutStone (int color, int x, int y, bool flip) {

    if (color != REVERSI_STONE_BLACK && color != REVERSI_STONE_WHITE) {
      return false;
    } else if (!(0 <= x && x < 8)) {
      return false;
    } else if (!(0 <= y && y < 8)) {
      return false;
    } else if (board_[x + 8 * y] != REVERSI_STONE_EMPTY) {
      return false;
    }

    std::array<int, 16> K{
      1, 0,
      1, 1,
      0, 1,
      -1, 1,
      -1, 0,
      -1, -1,
      0, -1,
      1, -1
    };

    std::array<ReversiStone, 64> board;
    board = board_;

    ReversiStone my_stone = ReversiStone(color);
    ReversiStone other_stone = ReversiStone(-color);

    bool flag = false;
    for (int i = 0; i < 8; ++i) {
      int kx = K[2 * i];
      int ky = K[2 * i + 1];
      int x1 = x + kx;
      int y1 = y + ky;

      if (!(0 <= x1 && x1 < 8 && 0 <= y1 && y1 < 8)) {
        continue;
      }
      if (board[x1 + 8 * y1] != other_stone) {
        continue;
      }
      x1 += kx;
      y1 += ky;

      int j = 2;
      while (0 <= x1 && x1 < 8 && 0 <= y1 && y1 < 8) {
        if (board[x1 + 8 * y1] == REVERSI_STONE_EMPTY) {
          break;
        }
        if (board[x1 + 8 * y1] == my_stone) {
          for (int l = 0; l < j; ++l) {
            int x2 = x + l * kx;
            int y2 = y + l * ky;
            board[x2 + 8 * y2] = my_stone;
          }
          flag = true;
          break;
        }
        ++j;
        x1 += kx;
        y1 += ky;
      }
    }
    if (flag) {
      if (flip) {
          board_ = board;
        if (color == REVERSI_STONE_BLACK) {
          status_ = REVERSI_STATUS_WHITE_TURN;
        } else if (color == REVERSI_STONE_WHITE) {
          status_ = REVERSI_STATUS_BLACK_TURN;
        }
      }
    }
    return flag;
  }

};

}

#include <apgame/reversi/ReversiServer.hpp>
