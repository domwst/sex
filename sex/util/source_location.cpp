#include "source_location.hpp"

#include <fmt/format.h>

namespace sex::util {

std::ostream &operator<<(std::ostream &out, const SourceLocation &location) {
  out << fmt::format("{:s}:{:d} in {:s}", location.Filename().data(),
                     location.Line(), location.Function().data());
  return out;
}

}  // namespace sex::util