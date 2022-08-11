#include "exit_status.hpp"

#include <sys/wait.h>

#include <sex/detail/syscall.hpp>

namespace sex {

ExitStatus::ExitStatus(int status) : status_(status) {
}

bool ExitStatus::IsExited() const {
  return WIFEXITED(status_);
}

bool ExitStatus::IsSignaled() const {
  return WIFSIGNALED(status_);
}

int ExitStatus::ExitCode() const {
  SEX_ASSERT(IsExited());
  return WEXITSTATUS(status_);
}

int ExitStatus::Signal() const  {
  SEX_ASSERT(IsSignaled());
  return WTERMSIG(status_);
}

}
