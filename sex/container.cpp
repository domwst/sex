#include "container.hpp"

#include <sex/util/once.hpp>

#include <sex/detail/syscall.hpp>

#include <unistd.h>

const fs::path Container::TmpSbox = fs::temp_directory_path() / "sbox";

namespace {

sex::util::Once initializer_;

void Initialize() {
  initializer_.Do([] {
    if (!fs::exists(Container::TmpSbox)) {
      fs::create_directory(Container::TmpSbox);
    }
  });
}

}

Container::Container(const std::string& name, const fs::path& containers_path)
  : ContainerPath_(containers_path / name) {

  Initialize();
  fs::create_directory(ContainerPath_);
}

void Container::Enter() const {
  SEX_SYSCALL(chroot(GetPath().c_str()));
  SEX_SYSCALL(chdir("/"));
}

const fs::path& Container::GetPath() const {
  return ContainerPath_;
}

Container::~Container() {
  fs::remove_all(ContainerPath_);
}
