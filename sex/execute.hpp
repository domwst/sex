#pragma once

#include <sex/util/fd-holder.hpp>
#include <sex/detail/syscall.hpp>

#include <linux/sched.h>
#include <syscall.h>
#include <sys/prctl.h>
#include <csignal>
#include <cstdint>
#include <sched.h>
#include <fstream>

namespace sex {

namespace execute_hooks {

struct None {
  void BeforeClone(clone_args&) {
  }

  void AfterCloneParent(const clone_args&, pid_t) {
  }

  void AfterCloneChild(const clone_args&) {
  }
};

struct Default : None {
  void BeforeClone(clone_args& args) {
    auto flags = args.flags;
    if (flags & CLONE_NEWUSER) {
      RememberUidGid();
    }
  }

  void AfterCloneChild(const clone_args& args) {
    auto flags = args.flags;
    if (flags & CLONE_NEWUSER) {
      CreateUidGidMappings();
    }
  }

 private:
  void RememberUidGid() {
    // Always successful
    previous_uid_ = getuid();
    previous_gid_ = getgid();
  }

  void CreateUidGidMappings() const {
    SYSCALL(prctl(PR_SET_DUMPABLE, 1, 42, 42, 42));

    SEX_ASSERT(std::ofstream("/proc/self/uid_map") << "0 " << previous_uid_ << " 1");
    SEX_ASSERT(std::ofstream("/proc/self/setgroups") << "deny");
    SEX_ASSERT(std::ofstream("/proc/self/gid_map") << "0 " << previous_gid_ << " 1");
  }

  uint previous_uid_;
  uint previous_gid_;
};

}

class ProcessKnob;
class ExecuteArgs;

template<class F, class Hooks = execute_hooks::Default>
ProcessKnob Execute(F&& f, ExecuteArgs args, Hooks&& hooks = execute_hooks::Default{});

struct ExitStatus {
  explicit ExitStatus(int status) : status_(status) {
  }

  [[nodiscard]] bool IsExited() const {
    return WIFEXITED(status_);
  }

  [[nodiscard]] bool IsSignaled() const {
    return WIFSIGNALED(status_);
  }

  [[nodiscard]] int ExitCode() const {
    SEX_ASSERT(IsExited());
    return WEXITSTATUS(status_);
  }

  [[nodiscard]] int Signal() const {
    SEX_ASSERT(IsSignaled());
    return WTERMSIG(status_);
  }

 private:
  int status_;
};

class ProcessKnob {
 public:
  [[nodiscard]] int GetPidfd() const {
    return pid_fd_.get();
  }

  ExitStatus Wait() && {  // NOLINT
    int status;
    SEX_ASSERT(SYSCALL(waitpid(pid_, &status, __WALL)) == pid_);
    return ExitStatus(status);
  }

  [[nodiscard]] int Pid() const {
    return pid_;
  }

  template<class F, class Hooks>
  friend ProcessKnob Execute(F&& f, ExecuteArgs args, Hooks&& hooks);

 private:
  ProcessKnob(int pid, FdHolder pid_fd)
    : pid_(pid), pid_fd_(std::move(pid_fd)) {
  }

  explicit ProcessKnob(int pid) : ProcessKnob(pid, FdHolder()) {
  }

  int pid_;
  FdHolder pid_fd_;
};

#pragma push_macro("EXECUTE_FLAG")

#define EXECUTE_FLAG(KnobName, Flag)  \
  ExecuteArgs& New##KnobName() {      \
    return SetFlag(Flag);             \
  }                                   \
                                      \
  ExecuteArgs& Same##KnobName() {     \
    return SetFlag(Flag);             \
  }

class ExecuteArgs {
 public:
  EXECUTE_FLAG(UserNS, CLONE_NEWUSER)

  EXECUTE_FLAG(MountNS, CLONE_NEWNS)

  EXECUTE_FLAG(NetworkNS, CLONE_NEWNET)

  EXECUTE_FLAG(IPCNS, CLONE_NEWIPC)

  EXECUTE_FLAG(CgroupNS, CLONE_NEWCGROUP)

  EXECUTE_FLAG(PidNS, CLONE_NEWPID)

  EXECUTE_FLAG(UTSNS, CLONE_NEWUTS)

  EXECUTE_FLAG(TimeNS, CLONE_NEWTIME)

  ExecuteArgs& IntoCgroup(int cfd) {
    args.cgroup = cfd;
    return SetFlag(CLONE_INTO_CGROUP);
  }

  ExecuteArgs& DiscardIntoCgroup() {
    args.cgroup = -1;
    return UnsetFlag(CLONE_INTO_CGROUP);
  }

  ExecuteArgs& CreatePidFD() {
    SetFlag(CLONE_PIDFD);
    return *this;
  }

  ExecuteArgs& DiscardPidFDCreation() {
    UnsetFlag(CLONE_PIDFD);
    return *this;
  }

  ExecuteArgs& SetChildExitSignal(int signal) {
    args.exit_signal = signal;
    return *this;
  }

  [[nodiscard]] const clone_args* GetCloneArgs() const {
    return &args;
  }

 private:
  ExecuteArgs& SetFlag(uint64_t flag) {
    args.flags |= flag;
    return *this;
  }

  ExecuteArgs& UnsetFlag(uint64_t flag) {
    args.flags &= ~flag;
    return *this;
  }

  clone_args args {
    .flags = 0,
    .pidfd = 0,
    .child_tid = 0,
    .parent_tid = 0,
    .exit_signal = SIGCHLD,
    .stack = 0,
    .stack_size = 0,
    .tls = 0,
    .set_tid = 0,
    .set_tid_size = 0,
    .cgroup = 0,
  };
};

#undef EXECUTE_FLAG
#pragma pop_macro("EXECUTE_FLAG")

template<class F, class Hooks>
ProcessKnob Execute(F&& f, ExecuteArgs args, Hooks&& hooks) {
  auto cl_args = *args.GetCloneArgs();

  int pidfd;
  if (cl_args.flags & CLONE_PIDFD) {
    cl_args.pidfd = reinterpret_cast<uint64_t>(&pidfd);
  }

  hooks.BeforeClone(cl_args);

  pid_t child_pid = SYSCALL(syscall(SYS_clone3, &cl_args, sizeof(cl_args)));

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
    pfd.reset(pidfd);
  }

  return {child_pid, std::move(pfd)};
}

}  // namespace sex
