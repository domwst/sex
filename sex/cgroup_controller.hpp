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

   AddBuilderFieldDefault(uint64_t, MemoryLimit,
                          NoLimit) // CAUTION: This REALLY slows down process execution
   AddBuilderFieldDefault(uint64_t, PidsLimit, NoLimit)
  };

  CgroupController() = delete;

  CgroupController(const CgroupController&) = delete;

  CgroupController(CgroupController&&) = delete;

  CgroupController(std::string_view cgroup_name, const Builder& options);

  void SetMemoryLimit(
    uint64_t newval); // CAUTION: This REALLY slows down process execution
  void SetPidsLimit(uint64_t newval);

  uint64_t GetCurrentMemory();

  void CgroupKill();

  FdHolder GetCgroupFd() const;

  const fs::path& GetCgroupPath() const;

  ~CgroupController();

  static const fs::path CgroupsPath;
  static const fs::path SboxCgroup;
  static const fs::path SubtreeControl;


 private:
  const fs::path CgroupPath_;

  static constexpr std::string_view memory_max = "memory.max";
  static constexpr std::string_view memory_current = "memory.current";
  static constexpr std::string_view pids_max = "pids.max";
  static constexpr std::string_view cgroup_kill = "cgroup.kill";
};

}
