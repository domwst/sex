#pragma once

#include <variant>
#include <exception>
#include <string>
#include <memory>

#include "sex/detail/exceptions/exception_concept.hpp"
#include "sex/detail/exceptions/syscall_error.hpp"
#include "macros.hpp"

namespace sex::util {

// TODO: Replace monostate with Unit
template<class TResult = std::monostate>
class [[nodiscard]] Result;

template<class T>
constexpr bool IsResult = false;

template<class T>
constexpr bool IsResult<Result<T>> = true;

template<class T>
concept CResult = IsResult<std::remove_cvref<T>>;

template<class TResult>
class [[nodiscard]] Result {
public:
  Result() = delete;

  [[nodiscard]] bool isError() const {
    return std::holds_alternative<std::exception_ptr>(result_);
  }

  [[nodiscard]] std::exception_ptr getError() const {
    return std::get<std::exception_ptr>(result_);
  }

  [[nodiscard]] bool isOk() const {
    return std::holds_alternative<TResult>(result_);
  }

  [[nodiscard]] TResult unwrap() const {
    ensure();
    return get<TResult>(result_);
  }

  void ensure() const {
    if (isError()) {
      std::rethrow_exception(get<std::exception_ptr>(result_));
    }
  }

  template<class F, class U = decltype(std::declval<F &&>()(std::declval<TResult>()))>
  auto then(F &&f) const {  // >>=/fmap
    if constexpr (CResult<U>) {
      if (isError()) {
        return U::ErrorFrom(*this);
      }
      return std::forward<F>(f)(unwrap());
    } else {
      if (isError()) {
        return Result<U>::ErrorFrom(*this);
      }
      return Result<U>::Ok(std::forward<F>(f)(unwrap()));
    }
  }

  static Result Ok(TResult result) {
    return Result(std::move(result));
  }

  template<class U = TResult, class = std::enable_if_t<std::is_same_v<U, std::monostate>>>
  static Result Ok() {
    return Result(std::monostate{});
  }

  template<CException Error>
  static Result Error(Error &&err) {
    return Result(std::make_exception_ptr(std::forward<Error>(err)));
  }

  static Result FromCurrentException() {
    return Result(std::current_exception());
  }

  template<class U>
  static Result ErrorFrom(const Result<U> &r) {
    SEX_ASSERT(r.isError());
    return Result(r.getError());
  }

private:
  explicit Result(TResult result) : result_(std::move(result)) {
  }

  explicit Result(std::exception_ptr error) : result_(std::move(error)) {
  }

  std::variant<TResult, std::exception_ptr> result_;
};

}  // namespace sex::util