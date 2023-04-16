#include "pipe.hpp"

#include <array>
#include <unistd.h>

namespace sex::util {

Pipe MakePipe(Pipe::Flags flags) {
  std::array<int, 2> fd{};
  SEX_SYSCALL(pipe2(fd.data(), static_cast<int>(flags) ^ O_CLOEXEC)).ensure();

  return Pipe{.in = FdHolder(fd[1]), .out = FdHolder(fd[0])};
};

}  // namespace sex::util
