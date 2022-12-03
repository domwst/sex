#pragma once

#include <ostream>
#include <chrono>

namespace sex::test::detail {

void PrintTimeSuffix(
  std::ostream& out,
  std::milli
) {
  out << "ms";
}

void PrintTimeSuffix(
  std::ostream& out,
  std::micro
) {
  out << "us";
}

void PrintTimeSuffix(
  std::ostream& out,
  std::nano
) {
  out << "ns";
}

}  // namespace sex::test::detail

template<class Rep, class Period>
std::ostream&
operator<<(std::ostream& out, std::chrono::duration<Rep, Period> dur) {
  out << dur.count();
  sex::test::detail::PrintTimeSuffix(out, Period{});
  return out;
}
