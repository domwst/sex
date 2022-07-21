#pragma once

#include <sex/util/source_location.hpp>

#include <string_view>

namespace sex::util {

[[noreturn]] void Panic(std::string_view message, SourceLocation location = SourceLocation::Current());

}  // namespace sex::util
