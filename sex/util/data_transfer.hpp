#pragma once

#include <sex/util/pipe.hpp>

namespace sex::util {

/**
 * Simple IPC Data transfer object utilizing pipes. Doesn't perform type checks, sends and receives data byte-wise
 */
class DataTransfer {
public:
  DataTransfer() : pipe_(sex::util::MakePipe()) {
  }

  template<class T>
  Result<> send(T v) {
    return SEX_SYSCALL(write(pipe_.in.getInt(), &v, sizeof(v))).then([](auto) { return std::monostate{}; });
  }

  template<class T>
  Result<T> receive() {
    T ret;
    return SEX_SYSCALL(read(pipe_.out.getInt(), &ret, sizeof(ret))).then([&](auto) { return ret; });
  }

private:
  sex::util::Pipe pipe_;
};

}  // namespace sex::util
