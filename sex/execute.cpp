#include "execute.hpp"

namespace sex {

ProcessKnob Execute(Routine f, ExecuteArgs args, IExecuteHooks& hooks) {
  auto cl_args = *args.GetCloneArgs();

  int pidfd;
  if (cl_args.flags & CLONE_PIDFD) {
    cl_args.pidfd = reinterpret_cast<uint64_t>(&pidfd);
  }

  hooks.BeforeClone(cl_args);

  pid_t child_pid = SEX_SYSCALL(syscall(SYS_clone3, &cl_args, sizeof(cl_args)));

  if (child_pid == 0) {
    hooks.AfterCloneChild(cl_args);
    try {
      f();
      _exit(0);
    } catch (...) {
      _exit(127);
    }
  }

  hooks.AfterCloneParent(cl_args, child_pid);
  FdHolder pfd;

  if (cl_args.flags & CLONE_PIDFD) {
    pfd.Reset(pidfd);
  }

  return {child_pid, std::move(pfd)};
}

ProcessKnob Execute(Routine f, ExecuteArgs args, IExecuteHooks&& hooks) {
  return Execute(std::move(f), args, hooks);
}

}  // sex
