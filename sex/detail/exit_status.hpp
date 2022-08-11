#pragma once

namespace sex {

class ExitStatus {
  friend class ProcessKnob;

 public:
  [[nodiscard]] bool IsExited() const;

  [[nodiscard]] bool IsSignaled() const;

  [[nodiscard]] int ExitCode() const;

  [[nodiscard]] int Signal() const;

 private:
  explicit ExitStatus(int status);

  int status_;
};

}  // sex
