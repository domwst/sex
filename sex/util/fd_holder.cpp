#include "fd_holder.hpp"

#include <sex/detail/syscall.hpp>

#include <iostream>
#include <unistd.h>

namespace sex {

FdHolder::FdHolder(int fd) noexcept: fd_(fd) {}

FdHolder::FdHolder() noexcept: FdHolder(NeutralValue) {}

FdHolder::FdHolder(FdHolder&& other) noexcept: FdHolder() {
  Swap(other);
}

void FdHolder::Swap(FdHolder& other) noexcept {
  std::swap(fd_, other.fd_);
}

FdHolder& FdHolder::operator=(FdHolder&& other) noexcept {
  Swap(other);
  return *this;
}

int FdHolder::Get() const noexcept {
  return fd_;
}

void FdHolder::Reset(int new_fd) noexcept {
  if (fd_ != NeutralValue) {
    SEX_SYSCALL(close(fd_));
  }
  fd_ = new_fd;
}

int FdHolder::Release() noexcept {
  int f = Get();
  fd_ = NeutralValue;
  return f;
}

FdHolder::~FdHolder() {
  Reset();
}

}
