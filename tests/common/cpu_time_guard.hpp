#pragma once

#include "stopwatch/cpu_stopwatch.hpp"

#include <boost/ut.hpp>

namespace sex::test {

class CpuTimeGuard {
 public:
  explicit CpuTimeGuard(CpuStopwatch::duration time_limit)
    : time_limit_(time_limit) {
  }

  ~CpuTimeGuard() {
    boost::ut::expect(stopwatch_.TimePassed() <= time_limit_);
  }

 private:
  CpuStopwatch stopwatch_;
  CpuStopwatch::duration time_limit_;
};

}  // namespace sex::test
