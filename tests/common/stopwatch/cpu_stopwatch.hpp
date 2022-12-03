#pragma once

#include "basic_stopwatch.hpp"

#include <chrono>
#include <ctime>
#include <ratio>

namespace sex::test {

namespace detail {

struct CpuClock {
  using rep = clock_t;
  using period = std::ratio<1, CLOCKS_PER_SEC>;
  using duration = std::chrono::duration<rep, period>;
  using time_point = std::chrono::time_point<CpuClock>;
  static constexpr bool is_steady = true;

  static auto now() {
    return time_point(duration(clock()));
  }
};

}  // namespace detail

using CpuStopwatch = BasicStopwatch<detail::CpuClock>;

}  // namespace sex::test
