#include <sex/util/fd_holder.hpp>
#include <sex/detail/syscall.hpp>

#include <unistd.h>
#include <fcntl.h>
#include <array>

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

[[nodiscard]] Pipe MakePipe(Pipe::Flags flags = Pipe::NONE) {
  std::array<int, 2> fd{};
  SEX_SYSCALL(pipe2(fd.data(), static_cast<int>(flags) ^ O_CLOEXEC));

  return Pipe{.in = FdHolder(fd[1]), .out = FdHolder(fd[0])};
};

}
