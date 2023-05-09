#pragma once

namespace sex {

class ExitStatus {
  friend class ProcessKnob;

 public:
  [[nodiscard]] bool isExited() const;

  [[nodiscard]] bool isSignaled() const;

  [[nodiscard]] int exitCode() const;

  [[nodiscard]] int signal() const;

 private:
  explicit ExitStatus(int status);

  int status_;
};

}  // sex
