#pragma once

#include <sex/detail/macros.hpp>
#include <sex/util/panic.hpp>
#include <sex/util/source_location.hpp>

#include <fmt/core.h>

#include <string_view>
#include <cstring>

#include "sex/util/result.hpp"

#define SEX_SYSCALL(expr) ::sex::detail::SyscallResult(#expr, expr)

namespace sex::detail {

template<class TResult>
util::Result<TResult> SyscallResult(
  std::string_view expression,
  TResult result,
  util::SourceLocation location = util::SourceLocation::Current()) {
  int error_code = errno;  // In case if following lines mess up error code
  if (SEX_UNLIKELY(int64_t(result) == -1)) {
    std::string error_message = fmt::format("{:s} failed: {:s} ({:d})\nAt {:s}:{:d} in function {:s}",
                                            expression.data(), strerror(error_code), error_code,
                                            location.Filename(), location.Line(), location.Function());
    return util::Result<TResult>::Error(ErrorMessageSyscallError(std::move(error_message), error_code));
  }
  return util::Result<TResult>::Ok(result);
}

}  // namespace sex::detail
