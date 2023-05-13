#include "process_knob.hpp"

#include "sex/detail/syscall.hpp"

#include <sys/wait.h>
#include <utility>

namespace sex::detail {

ProcessKnob::ProcessKnob(int pid, util::FdHolder pid_fd)
  : pid_(pid), pid_fd_(std::move(pid_fd)) {
}

ProcessKnob::ProcessKnob(int pid) : ProcessKnob(pid, util::FdHolder()) {
}

util::FileDescriptor ProcessKnob::getPidFd() const {
  return util::FileDescriptor(pid_fd_);
}

util::ExitStatus ProcessKnob::wait() && {  // NOLINT
  int status;
  SEX_ASSERT(waitpid(pid_, &status, __WALL) == pid_);
  if (WIFEXITED(status)) {
    return util::ExitStatus::Exited(WEXITSTATUS(status));
  }
  if (WIFSIGNALED(status)) {
    return util::ExitStatus::Signaled(WTERMSIG(status));
  }
  util::Panic(fmt::format("Wait status {} is not supported", status));
}

int ProcessKnob::getPid() const {
  return pid_;
}

util::Result<> ProcessKnob::sendSignal(int signum) {
  return SEX_SYSCALL(kill(getPid(), signum)).then([](auto) { return std::monostate{}; });
}

}  // sex::detail
