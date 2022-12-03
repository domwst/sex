#include "time_print.hpp"

namespace sex::test::detail {

void PrintTimeSuffix(std::ostream& out, std::milli) {
  out << "ms";
}

void PrintTimeSuffix(std::ostream& out, std::micro) {
  out << "us";
}

void PrintTimeSuffix(std::ostream& out, std::nano) {
  out << "ns";
}

}  // namespace sex::test::detail
