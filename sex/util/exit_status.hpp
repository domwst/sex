#pragma once

namespace sex::util {

class ExitStatus {
 public:
  ExitStatus() = default;

  enum class Type {
    Exited,
    Signaled,
  };

  [[nodiscard]] bool isExited() const;

  [[nodiscard]] bool isSignaled() const;

  [[nodiscard]] int getExitCode() const;

  [[nodiscard]] int getSignal() const;

  [[nodiscard]] Type getType() const;

  static ExitStatus Exited(int exitCode);
  static ExitStatus Signaled(int signal);

  bool operator==(const ExitStatus& other) const = default;

 private:
  ExitStatus(Type type, int value);

  Type type_{Type::Exited};
  int value_{0};  // TODO: Maybe reconsider default value
};

}  // sex::util
