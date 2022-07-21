#pragma once

#include <sex/detail/conditions.hpp>
#include <sex/util/panic.hpp>
#include <sex/util/source_location.hpp>

#include <fmt/core.h>

#include <string_view>
#include <cstring>

#define SYSCALL(expr) ::sex::detail::SyscallResult(#expr, expr, ::sex::detail::less(0))
#define SYSCALL_COND(expr, fail_cond) ::sex::detail::SyscallResult(#expr, expr, fail_cond)

namespace sex::detail {

auto SyscallResult(
  std::string_view expression,
  auto result,
  auto fail_condition,
  util::SourceLocation location = util::SourceLocation::Current()) {
  int error_code = errno;  // In case if following lines mess up error code
  if (fail_condition(result)) {
    Panic(fmt::format("{:s} failed: {:s}({:d})", expression, strerror(error_code), error_code), location);
  }
  return result;
}

}  // namespace sex::detail
