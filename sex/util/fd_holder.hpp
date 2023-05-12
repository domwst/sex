#pragma once

#include <sex/util/file_descriptor.hpp>

namespace sex::util {

class FdHolder {
 private:
  FileDescriptor fd_;

 public:
  explicit FdHolder(int fd) noexcept;

  FdHolder() noexcept = default;

  FdHolder(FdHolder&& other) noexcept;

  FdHolder(const FdHolder&) = delete;

  FdHolder& operator=(const FdHolder&) = delete;

  FdHolder& operator=(FdHolder&& other) noexcept;

  void swap(FdHolder& other) noexcept;

  [[nodiscard]] int getInt() const noexcept;

  // NOLINTNEXTLINE
  [[nodiscard]] operator FileDescriptor() const& noexcept;

  void reset(FileDescriptor new_fd = {});

  FileDescriptor release() noexcept;

  ~FdHolder();
};

}  // namespace sex::util
