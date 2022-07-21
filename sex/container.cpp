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

Container::Container(const std::string& name) : ContainerPath_(TmpSbox / name) {
    fs::create_directory(ContainerPath_);
}

void Container::Enter() const {
    SYSCALL(chroot(GetPath().c_str()));
    SYSCALL(chdir("/"));
}

const fs::path& Container::GetPath() const {
    return ContainerPath_;
}

Container::~Container() {
    fs::remove_all(ContainerPath_);
}
