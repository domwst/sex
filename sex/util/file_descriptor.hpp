#pragma once

#include <utility>

namespace sex::util {

// Non-owning lightweight file descriptor handle
class FileDescriptor {
 public:
  FileDescriptor() = default;

  explicit FileDescriptor(int fd) : fd_(fd) {
  }

  [[nodiscard]] explicit operator bool() const noexcept {
    return fd_ != None;
  }

  [[nodiscard]] explicit operator int() const noexcept {
    return getInt();
  }

  [[nodiscard]] int getInt() const noexcept {
    return fd_;
  }

  int reset(int fd = None) noexcept {
    return std::exchange(fd_, fd);
  }

 private:
  static constexpr int None = -1;

  int fd_ = None;
};

}  // sex::util
