#include "fd_holder.hpp"

#include <sex/detail/syscall.hpp>

#include <unistd.h>

namespace sex {

FdHolder::FdHolder(int fd) noexcept: fd_(fd) {}

FdHolder::FdHolder(FdHolder&& other) noexcept: FdHolder() {
  Swap(other);
}

void FdHolder::Swap(FdHolder& other) noexcept {
  std::swap(fd_, other.fd_);
}

int FdHolder::GetInt() const noexcept {
  return fd_.GetInt();
}

FdHolder& FdHolder::operator=(FdHolder&& other) noexcept {
  Swap(other);
  return *this;
}

FdHolder::operator FileDescriptor() const noexcept {
  return fd_;
}

void FdHolder::Reset(FileDescriptor new_fd) {
  if (fd_) {
    SEX_SYSCALL(close(fd_.GetInt())).ensure();
  }
  fd_ = new_fd;
}

FileDescriptor FdHolder::Release() noexcept {
  auto f = FileDescriptor(*this);
  fd_.Reset();
  return f;
}

FdHolder::~FdHolder() {
  Reset();
}

}
