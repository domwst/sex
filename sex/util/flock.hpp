#pragma once

#include <cassert>
#include <filesystem>
#include <fcntl.h>
#include <sys/file.h>

#include <sex/util/fd_holder.hpp>
#include <sex/detail/syscall.hpp>

namespace sex::util {

namespace fs = std::filesystem;

class FileLockGuard {
public:
  enum LockType {
    RLock,
    RWLock,
  };

  enum Unlocked {
    Unlocked,
  };

  FileLockGuard(const fs::path &path, LockType t) : FileLockGuard(path) {
    lock(t);
  }

  FileLockGuard(const fs::path& path, enum Unlocked) : FileLockGuard(path) {
  }

  void lock(LockType lock_type) {
    assert(current_state_ == State::Unlocked);
    int flag;
    switch (lock_type) {
      case RLock:
        flag = LOCK_SH;
        break;
      case RWLock:
        flag = LOCK_EX;
        break;
    }
    SEX_SYSCALL(flock(fd_.getInt(), flag)).ensure();
    current_state_ = toState(lock_type);
  }

  void unlock() {
    if (current_state_ == State::Unlocked) {
      return;
    }
    SEX_SYSCALL(flock(fd_.getInt(), LOCK_UN)).ensure();
    current_state_ = State::Unlocked;
  }

  // Technically unnecessary because open file would be closed and the lock would be released anyway
  ~FileLockGuard() {
    unlock();
  }

private:
  explicit FileLockGuard(const fs::path& path) : fd_(SEX_SYSCALL(open(path.c_str(), O_RDONLY)).unwrap()) {
  }

  FdHolder fd_;

  enum class State {
    RLock = FileLockGuard::RLock,
    RWLock = FileLockGuard::RWLock,
    Unlocked = FileLockGuard::Unlocked,
  };

  static State toState(LockType t) {
    switch (t) {
      case RLock:
        return State::RLock;
      case RWLock:
        return State::RWLock;
    }
  }

  State current_state_{State::Unlocked};
};

}  // namespace sex::util