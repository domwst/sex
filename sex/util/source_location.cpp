#include "source_location.hpp"

#include <fmt/core.h>
#include <fmt/format.h>

namespace sex::util {

constexpr size_t SourceLocation::Line() const {
  return line_;
}

constexpr size_t SourceLocation::Column() const {
  return column_;
}

constexpr std::string_view SourceLocation::Filename() const {
  return filename_;
}

constexpr std::string_view SourceLocation::Function() const {
  return function_name_;
}

std::ostream& operator<<(std::ostream& out, const SourceLocation& location) {
  out << fmt::format("{:s}:{:d} in {:s}", location.Filename().data(), location.Line(), location.Function().data());
  return out;
}

}  // sex::util
