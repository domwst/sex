#include <sex/util/syscall.hpp>

#include <sys/resource.h>

namespace sex::util {

enum class ResourceType {
  FileSize = RLIMIT_FSIZE,
  FileCount = RLIMIT_NOFILE,
  CpuTime = RLIMIT_CPU,
  CoreDumpSize = RLIMIT_CORE,
  AddressSpace = RLIMIT_AS,
  DataSegmentSize = RLIMIT_DATA,
  StackSize = RLIMIT_STACK,
};

Result<> SetRLimit(ResourceType tp, uint64_t softLimit, uint64_t hardLimit) {
  rlimit64 lim {
    .rlim_cur = softLimit,
    .rlim_max = hardLimit,
  };
  return SEX_SYSCALL(setrlimit64(int(tp), &lim)).discardValue();
}

Result<> SetRLimit(ResourceType tp, uint64_t limit) {
  return SetRLimit(tp, limit, limit);
}

struct ResourceLimit {
  uint64_t soft;
  uint64_t hard;
};

Result<ResourceLimit> GetRLimit(ResourceType tp) {
  rlimit64 lim{};
  auto getResult = [&lim](auto) -> ResourceLimit {
    return {
      .soft = lim.rlim_cur,
      .hard = lim.rlim_max,
    };
  };
  return SEX_SYSCALL(getrlimit64(int(tp), &lim)).then(getResult);
}

} // namespace sex::util