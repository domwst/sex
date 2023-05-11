#include "exit_status.hpp"

#include <sys/wait.h>

#include "sex/detail/syscall.hpp"

namespace sex::util {

ExitStatus::ExitStatus(int status) : status_(status) {
}

bool ExitStatus::isExited() const {
  return WIFEXITED(status_);
}

bool ExitStatus::isSignaled() const {
  return WIFSIGNALED(status_);
}

int ExitStatus::exitCode() const {
  SEX_ASSERT(isExited());
  return WEXITSTATUS(status_);
}

int ExitStatus::signal() const {
  SEX_ASSERT(isSignaled());
  return WTERMSIG(status_);
}

}  // namespace sex::detail
