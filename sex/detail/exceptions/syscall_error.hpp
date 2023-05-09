#pragma once

#include <sex/detail/exceptions/message_exception.hpp>

#include <fmt/format.h>
#include <memory>

struct SyscallError : virtual std::exception {};

struct ErrorCodeSyscallError : SyscallError, virtual MessageException {
  explicit ErrorCodeSyscallError(std::error_code errorCode) :
    MessageException(GenerateMessage(errorCode)),
    errorCode(errorCode) {
  }

  const std::error_code errorCode;

private:
  const std::string what_;

  static std::string GenerateMessage(std::error_code errorCode) {
    return fmt::format("Error: \"{}\" (error code {})", errorCode.message(), errorCode.value());
  }
};

class ErrorMessageSyscallError : public ErrorCodeSyscallError {
public:
  explicit ErrorMessageSyscallError(std::string what, int errorCode) :
    ErrorMessageSyscallError(std::move(what), std::error_code(errorCode, std::system_category())) {
  }

  explicit ErrorMessageSyscallError(std::string what, std::error_code errorCode) :
    MessageException(std::move(what)),
    ErrorCodeSyscallError(errorCode) {
  }
};