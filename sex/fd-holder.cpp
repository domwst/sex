#include "fd-holder.h"

#include <cstring>
#include <iostream>
#include <unistd.h>

FdHolder::FdHolder(int fd) : fd_(fd) {}
FdHolder::FdHolder() : FdHolder(NeutralValue) {}
FdHolder::FdHolder(FdHolder&& other) : FdHolder() {
    swap(other);
}

void FdHolder::swap(FdHolder& other) {
    std::swap(fd_, other.fd_);
}

FdHolder& FdHolder::operator=(FdHolder&& other) {
    swap(other);
    return *this;
}

int FdHolder::get() const {
    return fd_;
}

void FdHolder::reset(int new_fd) {
    if (fd_ != NeutralValue) {
        if (close(fd_) < 0) {
            std::cerr << "close failed: " << strerror(errno) << std::endl;
        }
    }
    fd_ = new_fd;
}

int FdHolder::release() {
    int f = get();
    fd_ = NeutralValue;
    return f;
}

FdHolder::~FdHolder() {
    reset();
}