#pragma once

#include "mount.hpp"

#include "sex/util/syscall.hpp"

#include <filesystem>
#include <sys/mount.h>

namespace fs = std::filesystem;

namespace sex::util {

struct TmpFsMount final : IMount {
  TmpFsMount(fs::path destination, uint64_t sizeBytes) : destination_(std::move(destination)), sizeBytes_(sizeBytes) {
  }

  Result<> mount() override {
    SEX_ASSERT(!detached_ && !mounted_);
    std::error_code errorCode;
    fs::create_directories(destination_, errorCode);
    if (errorCode) {
      return Result<>::Error(ErrorCodeSyscallError(errorCode));
    }
    auto opts = fmt::format("size={}", sizeBytes_);
    if (auto syscallResult = SEX_SYSCALL(::mount("tmpfs", destination_.c_str(), "tmpfs", 0, opts.c_str()));
      syscallResult.isError()) {
      return Result<>::ErrorFrom(syscallResult);
    }
    mounted_ = true;
    return Result<>::Ok();
  }

  Result<> unmount() override {
    SEX_ASSERT(!detached_ && mounted_);
    auto result = SEX_SYSCALL(umount(destination_.c_str()));
    if (result.isError()) {
      return Result<>::ErrorFrom(result);
    }
    mounted_ = false;
    return Result<>::Ok();
  }

  void detach() && override {
    SEX_ASSERT(!detached_);
    detached_ = true;
  }

  ~TmpFsMount() override {
    if (!detached_ && mounted_) {
      unmount().ensure();
    }
  }

private:
  fs::path destination_;
  uint64_t sizeBytes_;
  bool mounted_{false};
  bool detached_{false};
};

}  // namespace sex::util