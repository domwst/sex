#pragma once

#include <sex/util/source_location.hpp>
#include <sex/detail/exceptions/message_exception.hpp>

#include <string_view>

namespace sex::util {

struct PanicException : virtual MessageException {
  explicit PanicException(std::string msg);
};

[[noreturn]] void Panic(std::string_view message,
                        SourceLocation location = SourceLocation::Current());

}  // namespace sex::util
