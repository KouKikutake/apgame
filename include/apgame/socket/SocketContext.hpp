#pragma once

#include <apgame/socket/Socket.hpp>
#include <apgame/socket/Lock.hpp>

#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>

#include <mutex>

namespace apgame {

struct SocketContext {

  SocketContext (Socket & socket, boost::asio::io_service & io_service, boost::asio::yield_context & yield)
  : socket_(socket)
  , io_service_(io_service)
  , yield_(yield) {
  }

  template <class T>
  bool send (T const & data) {
    return socket_.send(data, yield_);
  }

  bool send (std::string const & data) {
    std::size_t size = data.size();
    if (!socket_.send(size, yield_)) {
      return false;
    }
    if (size == 0) {
      return true;
    }
    if (!socket_.send(data[0], size, yield_)) {
      return false;
    }
    return true;
  }

  bool send (std::vector<char> const & data) {
    return socket_.send(data[0], data.size(), yield_);
  }

  template <class T>
  bool send (T const & data, std::size_t size) {
    return socket_.send(data, size, yield_);
  }

  bool send (char const * data, std::size_t size) {
    return socket_.send(*data, size, yield_);
  }

  template <class T>
  bool receive (T & data) {
    return socket_.receive(data, yield_);
  }

  bool receive (std::string & data, std::size_t max) {
    std::size_t size;
    if (!socket_.receive(size, yield_)) {
      return false;
    }
    if (size > max) {
      return false;
    }
    data.resize(size);
    if (size == 0) {
      return true;
    }
    if (!socket_.receive(data[0], size, yield_)) {
      return false;
    }
    return true;
  }

  bool receive (std::vector<char> & data, std::size_t max) {
    std::size_t size;
    if (!socket_.receive(size, yield_)) {
      return false;
    }
    if (size > max) {
      return false;
    }
    data.resize(size);
    if (!socket_.receive(data[0], size, yield_)) {
      return false;
    }
    return true;
  }

  template <class T>
  bool receive (T & data, std::size_t size) {
    return socket_.receive(data, size, yield_);
  }

  bool receive (char * data, std::size_t size) {
    return socket_.receive(*data, size, yield_);
  }

  void close () {
    socket_.close();
  }

  Lock lock (std::mutex & mtx) {
    return Lock(io_service_, yield_, mtx);
  }

protected:
  Socket & socket_;
  boost::asio::io_service & io_service_;
  boost::asio::yield_context & yield_;
};

}
