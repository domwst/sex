#pragma once

#include <sex/util/panic.hpp>
#include <sex/util/source_location.hpp>

#include <fmt/core.h>

#include <string_view>
#include <cstring>
#include "syscall_result.hpp"

#define SEX_SYSCALL(expr) ::sex::detail::SyscallResult(#expr, expr)

#define SEX_LIKELY(expr) __builtin_expect((expr), 1)
#define SEX_UNLIKELY(expr) !SEX_LIKELY(!(expr))

#define SEX_ASSERT(expr)                                              \
do {                                                                  \
  if (SEX_UNLIKELY(!(expr))) {                                        \
    ::sex::util::Panic(fmt::format("{:s} is not satisfied", #expr));  \
  }                                                                   \
} while (0)


namespace sex::detail {

template<class TResult>
[[nodiscard]] SyscallResult<TResult> SyscallResult(
  std::string_view expression,
  TResult result,
  util::SourceLocation location = util::SourceLocation::Current()) {
  int error_code = errno;  // In case if following lines mess up error code
  if (SEX_UNLIKELY(int64_t(result) == -1)) {
    std::string error_message = fmt::format("{:s} failed: {:s} ({:d})\nAt {:s}:{:d} in function {:s}",
                                            expression.data(), strerror(error_code), error_code,
                                            location.Filename(), location.Line(), location.Function());
    return SyscallError(std::move(error_message));
  }
  return result;
}

}  // namespace sex::detail
