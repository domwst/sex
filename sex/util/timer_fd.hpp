#pragma once

#include <sex/detail/syscall.hpp>
#include <sex/util/fd_holder.hpp>

#include <chrono>
#include <sys/timerfd.h>
#include <unistd.h>

namespace sex::util {

class TimerFd : protected FdHolder {
 public:
  enum Flags {
    NONE = 0,
    NONBLOCKING = TFD_NONBLOCK,
    NO_CLOSE_ON_EXEC = TFD_CLOEXEC,
  };

  using Duration = std::chrono::nanoseconds;

  struct TimerSpec {
    Duration first_expiration;
    Duration interval;
  };

  explicit TimerFd(Flags flags = NONE)
    : FdHolder(SEX_SYSCALL(timerfd_create(CLOCK_MONOTONIC,
                                        flags ^ NO_CLOSE_ON_EXEC)).unwrap()) {
  }

  using FdHolder::operator FileDescriptor;

  uint64_t wait() const {
    uint64_t cnt = 0;
    auto ret = read(getInt(), &cnt, sizeof(cnt));
    SEX_ASSERT(ret == sizeof(cnt) || (ret == -1 && errno == EAGAIN));
    return cnt;
  }

  TimerSpec cancel() noexcept {
    return set(Duration::zero());
  }

  TimerSpec set(Duration first_expiration) noexcept {
    return set(first_expiration, Duration::zero());
  }

  TimerSpec set(TimerSpec spec) noexcept {
    itimerspec tmp = ToITimerSpec(spec);
    itimerspec prev{};

    SEX_SYSCALL(timerfd_settime(getInt(), 0, &tmp, &prev)).ensure();
    return FromITimerSpec(prev);
  }

  TimerSpec set(Duration first_expiration, Duration interval) noexcept {
    return set({.first_expiration = first_expiration, .interval = interval});
  }

  [[nodiscard]] TimerSpec getTimerSpec() const {
    itimerspec spec{};
    SEX_SYSCALL(timerfd_gettime(getInt(), &spec)).ensure();
    return FromITimerSpec(spec);
  }

 private:
  static timespec ToTimeSpec(Duration d) {
    constexpr auto nano = std::nano::den;

    auto sec = d.count() / nano;
    auto nsec = d.count() % nano;

    return {.tv_sec = sec, .tv_nsec = nsec};
  }

  static Duration FromTimeSpec(timespec d) {
    constexpr auto nano = std::nano::den;

    return Duration(d.tv_nsec + d.tv_sec * nano);
  }

  static TimerSpec FromITimerSpec(itimerspec spec) {
    return {
      .first_expiration = FromTimeSpec(spec.it_value),
      .interval = FromTimeSpec(spec.it_interval)
    };
  }

  static itimerspec ToITimerSpec(TimerSpec spec) {
    auto [firstExpiration, interval] = spec;

    itimerspec result = {
      .it_interval = ToTimeSpec(interval),
      .it_value = ToTimeSpec(firstExpiration),
    };

    return result;
  }
};

}  // sex::util
