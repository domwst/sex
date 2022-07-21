#include "panic.hpp"

#include <fmt/color.h>

#include <iostream>

namespace sex::util {

void Panic(std::string_view message, SourceLocation location) {
  std::cerr << fmt::format("{:s} ", fmt::styled("ERROR", fmt::fg(fmt::color::red)));
  std::cerr << " at " << location << ": " << message << std::endl;
  std::abort();
}

}  // namespace sex::util
