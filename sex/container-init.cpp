#include "container.hpp"

#include <filesystem>

namespace fs = std::filesystem;

namespace {
    struct ContainerDirectoryInitialiser {
        ContainerDirectoryInitialiser() {
            if (!fs::exists(Container::TmpSbox)) {
                fs::create_directory(Container::TmpSbox);
            }
        }
    } container_directory_initialiser;
}
