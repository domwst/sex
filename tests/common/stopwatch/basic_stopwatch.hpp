#pragma once

namespace sex::test {

template<class Clock>
class BasicStopwatch {
 public:
  using rep = typename Clock::rep;
  using period = typename Clock::period;
  using duration = typename Clock::duration;
  using time_point = typename Clock::time_point;
  static constexpr bool is_steady = Clock::is_steady;

  BasicStopwatch() : clock_(Clock()), start_(clock_.now()) {
  }

  auto TimePassed() {
    return clock_.now() - start_;
  }

  auto now() {
    return TimePassed();
  }

 private:
  Clock clock_;
  typename Clock::time_point start_;
};

}  // namespace sex::test
