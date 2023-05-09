#include "panic.hpp"

#include <fmt/color.h>

#include <iostream>
#include <sstream>
#include <utility>

namespace sex::util {

PanicException::PanicException(std::string msg) : MessageException(std::move(msg)) {}

void Panic(std::string_view message, SourceLocation location) {
  std::stringstream ss;
  ss << "Panic at " << location << ": \"" << message << "\"";
  throw PanicException(ss.str());
}

}  // namespace sex::util
