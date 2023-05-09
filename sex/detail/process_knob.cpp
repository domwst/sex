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

FileDescriptor ProcessKnob::getPidFd() const {
  return FileDescriptor(pid_fd_);
}

ExitStatus ProcessKnob::wait() && {  // NOLINT
  int status;
  SEX_ASSERT(waitpid(pid_, &status, __WALL) == pid_);
  return ExitStatus(status);
}

int ProcessKnob::getPid() const {
  return pid_;
}

}  // sex
