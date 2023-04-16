#pragma once

#include <variant>
#include <exception>
#include <string>

class SyscallError : std::exception {
public:
    explicit SyscallError(std::string what) : what_(std::move(what)) {
    }

    [[nodiscard]] const char* what() const noexcept override {
        return what_.data();
    }

private:
    std::string what_;
};

template<class TResult>
class SyscallResult {
public:
    SyscallResult(TResult result) : result_(std::move(result)) {  // NOLINT
    }

    SyscallResult(const SyscallError& error) : result_(error) {  // NOLINT
    }

    SyscallResult() = delete;

    [[nodiscard]] bool isError() const {
        return std::holds_alternative<SyscallError>(result_);
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
            throw get<SyscallError>(result_);
        }
    }

private:
    std::variant<TResult, SyscallError> result_;

    static_assert(!std::is_same_v<TResult, SyscallError>, "TResult can't be SyscallError");
};