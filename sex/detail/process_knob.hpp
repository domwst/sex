#pragma once

#include <sex/detail/exit_status.hpp>
#include <sex/util/fd_holder.hpp>
#include <sex/detail/routine.hpp>

namespace sex {

class ExecuteArgs;

class IExecuteHooks;

class ProcessKnob {
 public:
  [[nodiscard]] util::FileDescriptor getPidFd() const;

  ExitStatus wait()&&;

  [[nodiscard]] int getPid() const;

  friend ProcessKnob Execute(detail::Routine f, ExecuteArgs args, IExecuteHooks& hooks);

 private:
  ProcessKnob(int pid, util::FdHolder pid_fd);

  explicit ProcessKnob(int pid);

  int pid_;
  util::FdHolder pid_fd_;
};

}  // sex
