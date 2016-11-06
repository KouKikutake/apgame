#pragma once

#include <apgame/game/Game.hpp>

#include <mutex>
#include <string>
#include <unordered_set>

namespace apgame {

struct Room {

  Room (std::unique_ptr<Game> game)
  : game_(std::move(game)) {
    max_user_ = 32;
    num_round_ = 1;
  }

  Game * getGame () {
    return game_.get();
  }

  std::string const & getName () const noexcept {
    return name_;
  }

  void setName (std::string const & name) {
    std::lock_guard<std::mutex> lock(mtx_);
    name_ = name;
  }

  char const * getGameName () const noexcept {
    return game_name_.data();
  }

  void setGameName (std::string const & name) {
    std::lock_guard<std::mutex> lock(mtx_);
    game_name_ = name;
  }

  int getNumRound () const noexcept {
    return num_round_;
  }

  void setNumRound (int num_round) {
    std::lock_guard<std::mutex> lock(mtx_);
    num_round_ = num_round;
  }

  bool joinRoom (User & user, std::string const & game_name) {
    std::lock_guard<std::mutex> lock(mtx_);
    return game_->join(user);
  }

  void run (GameContext & game_context) {
    int num_round  = num_round_;
    for (int i = 0; i < num_round; ++i) {
      game_->run(game_context);
    }
  }

private:
  mutable std::mutex mtx_;
  std::string name_;
  std::string game_name_;
  std::size_t max_user_;
  std::size_t max_game_;
  int num_round_;
  std::unique_ptr<Game> game_;
};

}
