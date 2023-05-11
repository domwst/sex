#include "cgroup_controller.hpp"

#include <sex/detail/syscall.hpp>

#include <sex/util/once.hpp>
#include <sex/util/flock.hpp>

#include <fcntl.h>
#include <fstream>
#include <iostream>

namespace sex {

const fs::path CgroupController::CgroupsPath = "/sys/fs/cgroup";
const fs::path CgroupController::SboxCgroup =
  CgroupController::CgroupsPath / "sbox";
const fs::path CgroupController::SubtreeControl = "cgroup.subtree_control";

namespace {

sex::util::Once InitializeParentCgroup = [] {
  auto check = []() -> bool {
    return fs::exists(CgroupController::SboxCgroup);
  };

  util::FileLockGuard lk(CgroupController::SboxCgroup.parent_path(), util::FileLockGuard::RLock);
  if (check()) {
    return;
  }
  lk.unlock();
  lk.lock(util::FileLockGuard::RWLock);
  if (check()) {
    return;
  }

  constexpr std::string_view EssentialControllers = "+memory +pids";
  SEX_ASSERT((std::ofstream(
    CgroupController::CgroupsPath / CgroupController::SubtreeControl)
    << EssentialControllers).good());
  fs::create_directory(CgroupController::SboxCgroup);
  SEX_ASSERT((std::ofstream(
    CgroupController::SboxCgroup / CgroupController::SubtreeControl)
    << EssentialControllers).good());
};

}

CgroupController::CgroupController(std::string_view cgroupName,
                                   const Builder& options)
  : cgroupPath_(SboxCgroup / cgroupName) {

  InitializeParentCgroup();

  fs::create_directory(cgroupPath_);
  if (uint64_t memLimitMax = options.getMemoryLimitMax(); memLimitMax !=
                                                          Builder::NoLimit) {
    setMemoryLimitMax(memLimitMax);
  }
  if (uint64_t memLimitHigh = options.getMemoryLimitHigh(); memLimitHigh !=
                                                          Builder::NoLimit) {
    setMemoryLimitHigh(memLimitHigh);
  }
  if (uint64_t pidsLimit = options.getPidsLimit(); pidsLimit !=
                                                   Builder::NoLimit) {
    setPidsLimit(pidsLimit);
  }
}

void CgroupController::setMemoryLimitMax(uint64_t newVal) {
  std::ofstream out(cgroupPath_ / memoryMax);
  if (newVal == Builder::NoLimit) {
    out << "max";
  } else {
    out << newVal;
  }
  SEX_ASSERT(out.good());
}

void CgroupController::setMemoryLimitHigh(uint64_t newVal) {
  std::ofstream out(cgroupPath_ / memoryHigh);
  if (newVal == Builder::NoLimit) {
    out << "max";
  } else {
    out << newVal;
  }
  SEX_ASSERT(out.good());
}

void CgroupController::setPidsLimit(uint64_t newVal) {
  std::ofstream out(cgroupPath_ / pidsMax);
  if (newVal == Builder::NoLimit) {
    out << "max";
  } else {
    out << newVal;
  }
  SEX_ASSERT(out.good());
}

uint64_t CgroupController::getCurrentMemory() {
  uint64_t ret;
  std::ifstream inf(cgroupPath_ / memoryCurrent);
  inf >> ret;
  SEX_ASSERT(inf.good());
  return ret;
}

void CgroupController::killAll() {
  SEX_ASSERT((std::ofstream(cgroupPath_ / cgroupKill) << "1").good());
}

util::FdHolder CgroupController::getCgroupFd() const {
  return util::FdHolder(SEX_SYSCALL(open(getCgroupPath().c_str(), O_PATH | O_RDONLY | O_CLOEXEC)).unwrap());
}

void CgroupController::addProcess(int pid) {
  SEX_ASSERT((std::ofstream(cgroupPath_ / cgroupProcs) << pid).good());
}

void CgroupController::enter() {
  addProcess(getpid());
}

CgroupController::~CgroupController() {
  fs::remove(cgroupPath_);
}

const fs::path& CgroupController::getCgroupPath() const {
  return cgroupPath_;
}

}  // sex
