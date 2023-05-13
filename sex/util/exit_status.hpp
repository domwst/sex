#pragma once

namespace sex {

namespace detail {
class ProcessKnob;
}

namespace util {

class ExitStatus {
  friend class sex::detail::ProcessKnob;

 public:
  [[nodiscard]] bool isExited() const;

  [[nodiscard]] bool isSignaled() const;

  [[nodiscard]] int exitCode() const;

  [[nodiscard]] int signal() const;

  [[nodiscard]] bool isOk() const;

 private:
  explicit ExitStatus(int status);

  int status_;
};

}  // util
}  // sex
