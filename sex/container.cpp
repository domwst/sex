#include "container.hpp"

#include <sex/util/once.hpp>

#include <iostream>
#include <unistd.h>
#include <cstring>

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
    if (chroot(GetPath().c_str()) < 0) {
        std::cerr << "Failed to chroot: " << strerror(errno) << std::endl;
        _exit(127);
    }
    if (chdir("/") < 0) {
        std::cerr << "Failed to chdir: " << strerror(errno) << std::endl;
        _exit(127);
    }
}

const fs::path& Container::GetPath() const {
    return ContainerPath_;
}

Container::~Container() {
    fs::remove_all(ContainerPath_);
}
