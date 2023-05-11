#include "execute_hooks.hpp"

#include "sex/detail/syscall.hpp"

#include <linux/sched.h>
#include <sys/prctl.h>
#include <fstream>

namespace sex::util {

void ExecuteHooksDefault::BeforeClone(clone_args& args) {
  auto flags = args.flags;
  if (flags & CLONE_NEWUSER) {
    RememberUidGid();
  }
}

void ExecuteHooksDefault::AfterCloneChild(const clone_args& args) {
  auto flags = args.flags;
  if (flags & CLONE_NEWUSER) {
    CreateUidGidMappings();
  }
}

void ExecuteHooksDefault::AfterCloneParent(const clone_args&, pid_t) {
}

void ExecuteHooksDefault::RememberUidGid() {
  // Always successful
  previous_uid_ = getuid();
  previous_gid_ = getgid();
}

void ExecuteHooksDefault::CreateUidGidMappings() const {
  SEX_SYSCALL(prctl(PR_SET_DUMPABLE, 1, 42, 42, 42)).ensure();

  SEX_ASSERT(
    std::ofstream("/proc/self/uid_map") << "0 " << previous_uid_ << " 1");
  SEX_ASSERT(std::ofstream("/proc/self/setgroups") << "deny");
  SEX_ASSERT(
    std::ofstream("/proc/self/gid_map") << "0 " << previous_gid_ << " 1");
}

}
