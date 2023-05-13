#include "exit_status.hpp"

#include "sex/detail/syscall.hpp"

namespace sex::util {

bool ExitStatus::isExited() const {
  return getType() == Type::Exited;
}

bool ExitStatus::isSignaled() const {
  return getType() == Type::Signaled;
}

int ExitStatus::getExitCode() const {
  SEX_ASSERT(isExited());
  return value_;
}

int ExitStatus::getSignal() const {
  SEX_ASSERT(isSignaled());
  return value_;
}

ExitStatus::Type ExitStatus::getType() const {
  return type_;
}

ExitStatus::ExitStatus(sex::util::ExitStatus::Type type, int value) : type_(type), value_(value) {
}

ExitStatus ExitStatus::Exited(int exitCode) {
  return {Type::Exited, exitCode};
}

ExitStatus ExitStatus::Signaled(int signal) {
  return {Type::Signaled, signal};
}

}  // namespace sex::detail
