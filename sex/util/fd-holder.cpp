#include "fd-holder.hpp"

#include <sex/detail/syscall.hpp>

#include <iostream>
#include <unistd.h>

FdHolder::FdHolder(int fd) noexcept : fd_(fd) {}

FdHolder::FdHolder() noexcept : FdHolder(NeutralValue) {}

FdHolder::FdHolder(FdHolder&& other) noexcept : FdHolder() {
    swap(other);
}

void FdHolder::swap(FdHolder& other) noexcept {
    std::swap(fd_, other.fd_);
}

FdHolder& FdHolder::operator=(FdHolder&& other) noexcept {
    swap(other);
    return *this;
}

int FdHolder::get() const noexcept {
    return fd_;
}

void FdHolder::reset(int new_fd) noexcept {
    if (fd_ != NeutralValue) {
      SYSCALL(close(fd_));
    }
    fd_ = new_fd;
}

int FdHolder::release() noexcept {
    int f = get();
    fd_ = NeutralValue;
    return f;
}

FdHolder::~FdHolder() {
    reset();
}
