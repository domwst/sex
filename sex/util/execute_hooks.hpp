#pragma once

#include <unistd.h>
#include <cstdint>

struct clone_args;

namespace sex::util {

class IExecuteHooks {
 public:
  virtual void BeforeClone(clone_args& args) = 0;

  virtual void AfterCloneParent(const clone_args& args, pid_t child_pid) = 0;

  virtual void AfterCloneChild(const clone_args& args) = 0;
};


class ExecuteHooksDefault : public IExecuteHooks {
 public:
  void BeforeClone(clone_args& args) override;

  void AfterCloneChild(const clone_args& args) override;

  void AfterCloneParent(const clone_args& args, pid_t) override;

 private:
  void RememberUidGid();

  void CreateUidGidMappings() const;

  uint64_t previous_uid_{-1u};
  uint64_t previous_gid_{-1u};
};

}  // namespace sex::detail
