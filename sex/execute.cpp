#include <iostream>
#include "execute.hpp"

namespace sex {

ProcessKnob Execute(detail::Routine f, ExecuteArgs args, IExecuteHooks& hooks) {
  auto cl_args = *args.GetCloneArgs();

  int pidfd = -1;
  if (cl_args.flags & CLONE_PIDFD) {
    cl_args.pidfd = reinterpret_cast<uint64_t>(&pidfd);
  }

  hooks.BeforeClone(cl_args);

  pid_t child_pid = SEX_SYSCALL(syscall(SYS_clone3, &cl_args, sizeof(cl_args))).unwrap();

  if (child_pid == 0) {
    hooks.AfterCloneChild(cl_args);
    try {
      f();
      _exit(0);
    } catch(std::exception& e) {
      std::cerr << "Exception caught: " << e.what() << std::endl;
    } catch (...) {
    }
    _exit(127);
  }

  hooks.AfterCloneParent(cl_args, child_pid);
  util::FdHolder pfd;

  if (cl_args.flags & CLONE_PIDFD) {
    pfd.Reset(util::FileDescriptor(pidfd));
  }

  return {child_pid, std::move(pfd)};
}

ProcessKnob Execute(detail::Routine f, ExecuteArgs args, IExecuteHooks&& hooks) {
  return Execute(std::move(f), args, hooks);
}

}  // sex
