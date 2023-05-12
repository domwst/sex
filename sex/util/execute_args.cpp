#include "execute_args.hpp"
#include "sex/util/file_descriptor.hpp"

#include <csignal>

#define DEFINE_EXECUTE_FLAG(KnobName, Flag)     \
ExecuteArgs& ExecuteArgs::New##KnobName() {     \
  return SetFlag(Flag);                         \
}                                               \
                                                \
ExecuteArgs& ExecuteArgs::Same##KnobName() {    \
  return UnsetFlag(Flag);                       \
}

namespace sex::util {

ExecuteArgs::ExecuteArgs() : args({
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
}) {}

DEFINE_EXECUTE_FLAG(UserNS, CLONE_NEWUSER)

DEFINE_EXECUTE_FLAG(MountNS, CLONE_NEWNS)

DEFINE_EXECUTE_FLAG(NetworkNS, CLONE_NEWNET)

DEFINE_EXECUTE_FLAG(IPCNS, CLONE_NEWIPC)

DEFINE_EXECUTE_FLAG(CgroupNS, CLONE_NEWCGROUP)

DEFINE_EXECUTE_FLAG(PidNS, CLONE_NEWPID)

DEFINE_EXECUTE_FLAG(UTSNS, CLONE_NEWUTS)

DEFINE_EXECUTE_FLAG(TimeNS, CLONE_NEWTIME)

ExecuteArgs& ExecuteArgs::SameFiles() {
  SetFlag(CLONE_FILES);
  return *this;
}

ExecuteArgs& ExecuteArgs::NewFiles() {
  UnsetFlag(CLONE_FILES);
  return *this;
}

ExecuteArgs& ExecuteArgs::IntoCgroup(util::FileDescriptor cfd) {
  args.cgroup = cfd.getInt();
  return SetFlag(CLONE_INTO_CGROUP);
}

ExecuteArgs& ExecuteArgs::DiscardIntoCgroup() {
  args.cgroup = -1;
  return UnsetFlag(CLONE_INTO_CGROUP);
}

ExecuteArgs& ExecuteArgs::CreatePidFd() {
  return SetFlag(CLONE_PIDFD);
}

ExecuteArgs& ExecuteArgs::DiscardPidFdCreation() {
  return UnsetFlag(CLONE_PIDFD);
}

ExecuteArgs& ExecuteArgs::SetChildExitSignal(int signal) {
  args.exit_signal = signal;
  return *this;
}

const clone_args* ExecuteArgs::GetCloneArgs() const {
  return &args;
}

ExecuteArgs& ExecuteArgs::SetFlag(uint64_t flag) {
  args.flags |= flag;
  return *this;
}

ExecuteArgs& ExecuteArgs::UnsetFlag(uint64_t flag) {
  args.flags &= ~flag;
  return *this;
}

}  // sex
