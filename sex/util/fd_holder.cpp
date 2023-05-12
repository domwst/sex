#include "fd_holder.hpp"

#include <sex/detail/syscall.hpp>

#include <unistd.h>

namespace sex::util {

FdHolder::FdHolder(int fd) noexcept: fd_(fd) {}

FdHolder::FdHolder(FdHolder&& other) noexcept: FdHolder() {
  swap(other);
}

void FdHolder::swap(FdHolder& other) noexcept {
  std::swap(fd_, other.fd_);
}

int FdHolder::getInt() const noexcept {
  return fd_.getInt();
}

FdHolder& FdHolder::operator=(FdHolder&& other) noexcept {
  swap(other);
  return *this;
}

FdHolder::operator FileDescriptor() const& noexcept {
  return fd_;
}

void FdHolder::reset(FileDescriptor new_fd) {
  if (fd_) {
    SEX_SYSCALL(close(fd_.getInt())).ensure();
  }
  fd_ = new_fd;
}

FileDescriptor FdHolder::release() noexcept {
  auto f = FileDescriptor(*this);
  fd_.reset();
  return f;
}

FdHolder::~FdHolder() {
  reset();
}

}  // namespace sex::util
