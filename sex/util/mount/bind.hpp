#pragma once

#include "mount.hpp"
#include "sex/util/syscall.hpp"

#include <sys/mount.h>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

namespace sex::util {

const auto SourceIsADirectory = MessageException("Source is a directory");
const auto SourceIsNotADirectory = MessageException("Source is not a directory");
const auto SourceNotFound = MessageException("Source not found");
const auto UnableToCreateOrOpenDestination = MessageException("Unable to create or open destination");

namespace detail {

struct FileBindFunctions {
  static Result<> CheckAndSetupDestination(const fs::path& source, const fs::path& destination) {
    std::error_code errorCode;
    auto status = fs::status(source, errorCode);
    if (errorCode) {
      return Result<>::Error(ErrorCodeSyscallError(errorCode));
    }
    if (status.type() == fs::file_type::not_found) {
      return Result<>::Error(SourceNotFound);
    }
    if (status.type() == fs::file_type::directory) {
      return Result<>::Error(SourceIsADirectory);
    }
    fs::create_directories(destination.parent_path(), errorCode);
    if (errorCode) {
      return Result<>::Error(ErrorCodeSyscallError(errorCode));
    }
    bool accessed = std::ofstream(destination.string()).good();
    if (!accessed) {
      return Result<>::Error(UnableToCreateOrOpenDestination);
    }
    return Result<>::Ok();
  }
};

struct DirectoryBindFunctions {
  static Result<> CheckAndSetupDestination(const fs::path& source, const fs::path& destination) {
    std::error_code errorCode;
    auto status = fs::status(source, errorCode);
    if (errorCode) {
      return Result<>::Error(ErrorCodeSyscallError(errorCode));
    }
    if (status.type() == fs::file_type::not_found) {
      return Result<>::Error(SourceNotFound);
    }
    if (status.type() != fs::file_type::directory) {
      return Result<>::Error(SourceIsNotADirectory);
    }
    fs::create_directories(destination, errorCode);
    if (errorCode) {
      return Result<>::Error(ErrorCodeSyscallError(errorCode));
    }
    return Result<>::Ok();
  }
};

template<class BindFunctions> requires requires(fs::path source, fs::path destination) {  // WTF?!
  { BindFunctions::CheckAndSetupDestination(source, destination) } -> std::same_as<Result<>>;
}
struct BindImpl final: IMount {
public:
  enum Flags {
    NONE = 0,
    NO_REC = MS_REC,  // Applicable to directories only
    ALLOW_WRITE = MS_RDONLY,
  };

  BindImpl(fs::path source, fs::path destination, Flags flags = NONE) :
          source_(std::move(source)), destination_(std::move(destination)), mountFlags_(flags ^ defaultFlags) {
  }

  Result<> mount() override {
    SEX_ASSERT(!mounted_ && !detached_);
    if (auto checkResult = BindFunctions::CheckAndSetupDestination(source_, destination_); checkResult.isError()) {
      return Result<>::ErrorFrom(checkResult);
    }
    auto result = SEX_SYSCALL(::mount(source_.c_str(), destination_.c_str(), "none", mountFlags_, ""));
    if (result.isError()) {
      return Result<>::ErrorFrom(result);
    }
    if (mountFlags_ & MS_RDONLY) {
      result = SEX_SYSCALL(::mount("none", destination_.c_str(), NULL, mountFlags_ | MS_REMOUNT, NULL));
      if (result.isError()) {
        return Result<>::ErrorFrom(result);
      }
    }
    mounted_ = true;
    return Result<>::Ok();
  }

  Result<> unmount() override {
    SEX_ASSERT(mounted_ && !detached_);

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

  ~BindImpl() override {
    if (!detached_ && mounted_) {
      unmount().ensure();
    }
  }

private:
  const fs::path source_;
  const fs::path destination_;
  const int mountFlags_;

  bool mounted_ = false;
  bool detached_ = false;

  static constexpr int defaultFlags = MS_BIND | MS_REC | MS_RDONLY;
};

}

using FileBindMount = detail::BindImpl<detail::FileBindFunctions>;
using DirectoryBindMount = detail::BindImpl<detail::DirectoryBindFunctions>;

} // namespace sex::util