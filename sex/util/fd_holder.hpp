#pragma once

#include <sex/util/file_descriptor.hpp>

namespace sex {

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

  void Swap(FdHolder& other) noexcept;

  [[nodiscard]] int GetInt() const noexcept;

  // NOLINTNEXTLINE
  [[nodiscard]] operator FileDescriptor() const& noexcept;

  void Reset(FileDescriptor new_fd = {});

  FileDescriptor Release() noexcept;

  ~FdHolder();
};

}
