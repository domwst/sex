#pragma once

#include "sex/util/builder_helper.hpp"
#include "sex/util/fd_holder.hpp"

#include <sex/util/source_location.hpp>

#include <filesystem>
#include <string_view>

namespace fs = std::filesystem;

namespace sex {

class CgroupController {
 public:
  class Builder {
   public:
    constexpr static uint64_t NoLimit = -1ull;

    Builder() = default;

    Builder(const Builder& other) = default;

    AddBuilderFieldDefault(uint64_t, MemoryLimitHigh, NoLimit)
    AddBuilderFieldDefault(uint64_t, MemoryLimitMax, NoLimit)
    AddBuilderFieldDefault(uint64_t, PidsLimit, NoLimit)

  public:
    struct CpuWindowLimit {
      uint64_t limitUS = NoLimit;
      uint64_t windowUS = NoLimit;
    };

    Builder& setCpuWindowLimit(std::chrono::microseconds limit, std::chrono::microseconds window) {
      cpuWindowLimit = {
        .limitUS = static_cast<uint64_t>(limit.count()),
        .windowUS = static_cast<uint64_t>(window.count()),
      };
      return *this;
    }

    [[nodiscard]] CpuWindowLimit getCpuWindowLimit() const {
      return cpuWindowLimit;
    }

  private:
    CpuWindowLimit cpuWindowLimit;
  };

  struct CpuUsage {
    std::chrono::microseconds system;
    std::chrono::microseconds user;
    std::chrono::microseconds total;
  };

  CgroupController() = delete;

  CgroupController(const CgroupController&) = delete;

  CgroupController(CgroupController&&) = delete;

  CgroupController(std::string_view cgroupName, const Builder& options);

  void setMemoryLimitHigh(uint64_t newVal);
  void setMemoryLimitMax(uint64_t newVal);
  void setPidsLimit(uint64_t newVal);
  void setCpuWindowLimit(std::chrono::microseconds limit, std::chrono::microseconds window);

  void addProcess(int pid);
  void enter();

  [[nodiscard]] uint64_t getCurrentMemory() const;
  [[nodiscard]] CpuUsage getCpuUsage() const;

  void killAll();

  [[nodiscard]] util::FdHolder getCgroupFd() const;

  [[nodiscard]] const fs::path& getCgroupPath() const;

  ~CgroupController();

  static const fs::path CgroupsPath;
  static const fs::path SboxCgroup;
  static const fs::path SubtreeControl;


 private:
  const fs::path cgroupPath_;

  [[nodiscard]] fs::path getControllerPath(std::string_view controller) const;

  static constexpr std::string_view memoryMax = "memory.max";
  static constexpr std::string_view memoryHigh = "memory.high";
  static constexpr std::string_view memoryCurrent = "memory.current";
  static constexpr std::string_view pidsMax = "pids.max";
  static constexpr std::string_view cgroupKill = "cgroup.kill";
  static constexpr std::string_view cgroupProcs = "cgroup.procs";
  static constexpr std::string_view cpuMax = "cpu.max";
  static constexpr std::string_view cpuStats = "cpu.stat";
};

}
