#pragma once

#include "file_descriptor.hpp"

#include <linux/sched.h>
#include <cstdint>

namespace sex::util {

#pragma push_macro("DECLARE_EXECUTE_FLAG")

#define DECLARE_EXECUTE_FLAG(KnobName)  \
  ExecuteArgs& New##KnobName();         \
                                        \
  ExecuteArgs& Same##KnobName();

class ExecuteArgs {
 public:
  ExecuteArgs();

  DECLARE_EXECUTE_FLAG(UserNS)

  DECLARE_EXECUTE_FLAG(MountNS)

  DECLARE_EXECUTE_FLAG(NetworkNS)

  DECLARE_EXECUTE_FLAG(IPCNS)

  DECLARE_EXECUTE_FLAG(CgroupNS)

  DECLARE_EXECUTE_FLAG(PidNS)

  DECLARE_EXECUTE_FLAG(UTSNS)

  DECLARE_EXECUTE_FLAG(TimeNS)

  ExecuteArgs& SameFiles();

  ExecuteArgs& NewFiles();

  ExecuteArgs& IntoCgroup(util::FileDescriptor cfd);

  ExecuteArgs& DiscardIntoCgroup();

  ExecuteArgs& CreatePidFd();

  ExecuteArgs& DiscardPidFdCreation();

  ExecuteArgs& SetChildExitSignal(int signal);

  [[nodiscard]] const clone_args* GetCloneArgs() const;

 private:
  ExecuteArgs& SetFlag(uint64_t flag);

  ExecuteArgs& UnsetFlag(uint64_t flag);

  clone_args args;
};

#undef DECLARE_EXECUTE_FLAG
#pragma pop_macro("DECLARE_EXECUTE_FLAG")

}  // sex::util
