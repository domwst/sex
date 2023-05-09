#pragma once

#include <exception>
#include <string>

struct MessageException : virtual std::exception {
  explicit MessageException(std::string message) : message_(std::move(message)) {
  }

  [[nodiscard]] const char* what() const noexcept override {
    return message_.data();
  }

private:
  const std::string message_;
};