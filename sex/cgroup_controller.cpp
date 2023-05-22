#include "cgroup_controller.hpp"

#include <sex/detail/syscall.hpp>

#include <sex/util/once.hpp>
#include <sex/util/flock.hpp>

#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <map>

namespace {

std::map<std::string, uint64_t> ReadFlatKeyedFile(std::istream &in) {
  std::string key, v;
  std::map<std::string, uint64_t> result;
  while (in >> key >> v) {
    uint64_t value = -1;
    if (v != "max") {
      value = std::stoull(v);
    }
    result[key] = value;
  }
  return result;
}

}

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

  constexpr std::string_view EssentialControllers = "+memory +pids +cpu";
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

  if (auto cpuWindowLimit = options.getCpuWindowLimit(); cpuWindowLimit.limitUS != Builder::NoLimit) {
    setCpuWindowLimit(
      std::chrono::microseconds(cpuWindowLimit.limitUS),
      std::chrono::microseconds(cpuWindowLimit.windowUS)
    );
  }
}

void CgroupController::setMemoryLimitMax(uint64_t newVal) {
  std::ofstream out(getControllerPath(memoryMax));
  if (newVal == Builder::NoLimit) {
    out << "max";
  } else {
    out << newVal;
  }
  SEX_ASSERT(out.good());
}

void CgroupController::setMemoryLimitHigh(uint64_t newVal) {
  std::ofstream out(getControllerPath(memoryHigh));
  if (newVal == Builder::NoLimit) {
    out << "max";
  } else {
    out << newVal;
  }
  SEX_ASSERT(out.good());
}

void CgroupController::setPidsLimit(uint64_t newVal) {
  std::ofstream out(getControllerPath(pidsMax));
  if (newVal == Builder::NoLimit) {
    out << "max";
  } else {
    out << newVal;
  }
  SEX_ASSERT(out.good());
}

uint64_t CgroupController::getCurrentMemory() const {
  uint64_t ret;
  std::ifstream inf(getControllerPath(memoryCurrent));
  inf >> ret;
  SEX_ASSERT(inf.good());
  return ret;
}

CgroupController::CpuUsage CgroupController::getCpuUsage() const {
  static const std::string systemUsage = "system_usec";
  static const std::string userUsage = "user_usec";
  static const std::string totalUsage = "usage_usec";

  std::ifstream inf(getControllerPath(cpuStats));
  const auto entries = ReadFlatKeyedFile(inf);
  SEX_ASSERT(
    entries.contains(systemUsage) &&
    entries.contains(userUsage) &&
    entries.contains(totalUsage)
  );

  return {
    .system = std::chrono::microseconds(entries.at(systemUsage)),
    .user = std::chrono::microseconds(entries.at(userUsage)),
    .total = std::chrono::microseconds(entries.at(totalUsage)),
  };
}

void CgroupController::killAll() {
  SEX_ASSERT((std::ofstream(getControllerPath(cgroupKill)) << "1").good());
}

util::FdHolder CgroupController::getCgroupFd() const {
  return util::FdHolder(SEX_SYSCALL(open(getCgroupPath().c_str(), O_PATH | O_RDONLY | O_CLOEXEC)).unwrap());
}

void CgroupController::addProcess(int pid) {
  SEX_ASSERT((std::ofstream(getControllerPath(cgroupProcs)) << pid).good());
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

void CgroupController::setCpuWindowLimit(std::chrono::microseconds limit, std::chrono::microseconds window) {
  SEX_ASSERT((std::ofstream(getControllerPath(cpuMax)) << limit.count() << " " << window.count()).good());
}

fs::path CgroupController::getControllerPath(std::string_view controller) const {
  return getCgroupPath() / controller;
}

}  // sex
