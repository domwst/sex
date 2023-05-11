#pragma once

#include "sex/util/exit_status.hpp"
#include "sex/util/fd_holder.hpp"
#include "routine.hpp"
#include <sex/util/result.hpp>

namespace sex {

namespace util {

class ExecuteArgs;
class IExecuteHooks;

}  // namespace util

namespace detail {

class ProcessKnob;

}  // namespace detail

detail::ProcessKnob Execute(detail::Routine f, util::ExecuteArgs args, util::IExecuteHooks& hooks);

namespace detail {

class ProcessKnob {
public:
  [[nodiscard]] util::FileDescriptor getPidFd() const;

  util::ExitStatus wait() &&;

  [[nodiscard]] int getPid() const;

  util::Result<> sendSignal(int signum);

  friend ProcessKnob sex::Execute(detail::Routine f, util::ExecuteArgs args, util::IExecuteHooks& hooks);

private:
  ProcessKnob(int pid, util::FdHolder pid_fd);

  explicit ProcessKnob(int pid);

  int pid_;
  util::FdHolder pid_fd_;
};

}  // detail
}  // sex