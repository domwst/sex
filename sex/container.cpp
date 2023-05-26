#include "container.hpp"

#include <sex/util/once.hpp>

#include "sex/util/syscall.hpp"
#include <sex/util/flock.hpp>

#include <unistd.h>

namespace sex {

const fs::path Container::TmpSbox = (fs::temp_directory_path() / "sbox").string();

namespace {

sex::util::Once Initialize = [] {
  auto check = []() -> bool {
    if (fs::exists(Container::TmpSbox)) {
      return true;
    }
    return false;
  };

  sex::util::FileLockGuard lk(Container::TmpSbox.parent_path(), sex::util::FileLockGuard::RLock);
  if (check()) {
    return;
  }
  lk.unlock();
  lk.lock(sex::util::FileLockGuard::RWLock);
  if (check()) {
    return;
  }

  fs::create_directory(Container::TmpSbox);
};

}

Container::Container(const std::string &name)
        : Container(name, TmpSbox) {
}

Container::Container(const std::string &name, const fs::path &containers_path)
        : containerPath_(containers_path / name) {

  Initialize();
  fs::create_directory(containerPath_);
}

void Container::enter() const {
  SEX_SYSCALL(chroot(getPath().c_str())).ensure();
  SEX_SYSCALL(chdir("/")).ensure();
}

void Container::remove() {
  assert(!removed_);
  fs::remove_all(containerPath_);
  removed_ = true;
}

const fs::path &Container::getPath() const {
  return containerPath_;
}

Container::~Container() {
  if (!removed_) {
    remove();
  }
}

}  // namespace sex
