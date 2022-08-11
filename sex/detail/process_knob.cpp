#include "process_knob.hpp"

#include <sex/detail/syscall.hpp>

#include <sys/wait.h>
#include <utility>

namespace sex {

ProcessKnob::ProcessKnob(int pid, FdHolder pid_fd)
  : pid_(pid), pid_fd_(std::move(pid_fd)) {
}

ProcessKnob::ProcessKnob(int pid) : ProcessKnob(pid, FdHolder()) {
}

int ProcessKnob::GetPidFd() const {
  return pid_fd_.get();
}

ExitStatus ProcessKnob::Wait()&& {  // NOLINT
  int status;
  SEX_ASSERT(waitpid(pid_, &status, __WALL) == pid_);
  return ExitStatus(status);
}

int ProcessKnob::Pid() const {
  return pid_;
}

}  // sex
