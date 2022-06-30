#include "cgroup-controller.h"

#include <filesystem>
#include <fstream>
#include <string_view>

namespace fs = std::filesystem;

namespace {
    struct CgroupInitialiser {
        CgroupInitialiser() {
            if (fs::exists(CgroupController::SboxCgroup)) {
                return;
            }
            constexpr std::string_view EssentialControllers = "+memory +pids";
            std::ofstream(CgroupController::CgroupsPath / CgroupController::SubtreeControl) << EssentialControllers;
            fs::create_directory(CgroupController::SboxCgroup);
            std::ofstream(CgroupController::SboxCgroup / CgroupController::SubtreeControl) << EssentialControllers;
        }
    } cgroup_initialiser;
}

