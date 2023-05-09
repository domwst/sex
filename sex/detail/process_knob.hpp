#pragma once

#include <sex/detail/exit_status.hpp>
#include <sex/util/fd_holder.hpp>
#include <sex/detail/routine.hpp>

namespace sex {

class ExecuteArgs;

class IExecuteHooks;

class ProcessKnob {
 public:
  [[nodiscard]] FileDescriptor getPidFd() const;

  ExitStatus wait()&&;

  [[nodiscard]] int getPid() const;

  friend ProcessKnob Execute(Routine f, ExecuteArgs args, IExecuteHooks& hooks);

 private:
  ProcessKnob(int pid, FdHolder pid_fd);

  explicit ProcessKnob(int pid);

  int pid_;
  FdHolder pid_fd_;
};

}  // sex
