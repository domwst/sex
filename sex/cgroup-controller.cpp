#include "cgroup-controller.h"

#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <string_view>
#include <unistd.h>

const fs::path CgroupController::CgroupsPath = "/sys/fs/cgroup";
const fs::path CgroupController::SboxCgroup = CgroupController::CgroupsPath / "sbox";
const fs::path CgroupController::SubtreeControl = "cgroup.subtree_control";

CgroupController::CgroupController(const std::string& cgroup_name, const Builder& options)
    : CgroupPath_(SboxCgroup / cgroup_name) {
    fs::create_directory(CgroupPath_);
    if (uint64_t mem_limit = options.GetMemoryLimit(); mem_limit != Builder::NoLimit) {
        ChangeMemoryLimit(mem_limit);
    }
    if (uint64_t pids_limit = options.GetPidsLimit(); pids_limit != Builder::NoLimit) {
        ChangePidsLimit(pids_limit);
    }
}

void CgroupController::ChangeMemoryLimit(uint64_t newval) {
    std::ofstream out(CgroupPath_ / memory_max);
    if (newval == Builder::NoLimit) {
        out << "max";
    } else {
        out << newval;
    }
}

void CgroupController::ChangePidsLimit(uint64_t newval) {
    std::ofstream out(CgroupPath_ / pids_max);
    if (newval == Builder::NoLimit) {
        out << "max";
    } else {
        out << newval;
    }
}

uint64_t CgroupController::GetCurrentMemory() {
    uint64_t ret;
    std::ifstream(CgroupPath_ / memory_current) >> ret;
    return ret;
}

void CgroupController::CgroupKill() {
    std::ofstream(CgroupPath_ / cgroup_kill) << "1";
}

FdHolder CgroupController::GetCgroupFd() const {
    int fd = open(GetCgroupPath().c_str(), O_PATH | O_RDONLY | O_CLOEXEC);
    if (fd == -1) {
        std::cerr << "Failed to open cgroup directory [" << GetCgroupPath() << "]: "
            << strerror(errno) << std::endl;
        return FdHolder();
    }
    return FdHolder(fd);
}

CgroupController::~CgroupController() {
    fs::remove(CgroupPath_);
}

const fs::path& CgroupController::GetCgroupPath() const {
    return CgroupPath_;
}
