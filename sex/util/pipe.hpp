#pragma once

#include <sex/util/fd_holder.hpp>
#include <sex/detail/syscall.hpp>

#include <fcntl.h>

namespace sex::util {

struct Pipe {
  FdHolder in;
  FdHolder out;

  enum Flags : int {
    NONE = 0,
    NONBLOCKING = O_NONBLOCK,
    NO_CLOSE_ON_EXEC = O_CLOEXEC,
  };
};

[[nodiscard]] Pipe MakePipe(Pipe::Flags flags = Pipe::NONE);

}
