#include "common/time_print.hpp"
#include "common/cpu_time_guard.hpp"
#include "common/stopwatch/high_resolution_stopwatch.hpp"

#include <sex/util/timer_fd.hpp>
#include <sex/util/pipe.hpp>
#include <sex/execute.hpp>

#include <boost/ut.hpp>

#include <thread>
#include <fcntl.h>

namespace ut = boost::ut;

using Duration = sex::util::TimerFd::Duration;

static constexpr Duration time_eps = std::chrono::milliseconds(10);

void ExpectExpireIn(sex::util::TimerFd& timer, Duration expected,
                    Duration precision = time_eps) {

  using namespace ut;
  using namespace sex;

  auto start = std::chrono::high_resolution_clock::now();
  expect(eq(timer.wait(), size_t(1)));
  auto end = std::chrono::high_resolution_clock::now();

  auto elapsed = end - start;
  auto deviation = abs(elapsed - expected);
  expect(that % deviation <= precision);
}

static ut::suite timerFd = [] {
  using namespace ut;
  using namespace sex;
  using namespace util;
  using namespace std::chrono_literals;

  "simple_1"_test = [] {
    TimerFd timer;
    timer.set(200ms);
    ExpectExpireIn(timer, 200ms);
  };

  "simple_2"_test = [] {
    constexpr Duration first_expiration = 200ms;
    constexpr Duration interval = 50ms;

    TimerFd timer;
    timer.set(first_expiration, interval);
    ExpectExpireIn(timer, first_expiration);
    for (int i = 0; i < 3; ++i) {
      ExpectExpireIn(timer, interval);
    }
    auto spec = timer.cancel();

    expect(that % spec.interval == interval);
    expect(that % spec.first_expiration <= interval);
    expect(that % spec.first_expiration >= interval - time_eps);
  };

  "non_blocking"_test = [] {
    constexpr Duration delay = 100ms;

    TimerFd timer(TimerFd::NONBLOCKING);
    timer.set(delay);
    expect(that % timer.wait() == size_t(0));
    std::this_thread::sleep_for(delay + 10ms);
    expect(that % timer.wait() == size_t(1));
  };

  "blocking_indefinitely"_test = [] {
    auto pipe = sex::util::MakePipe();

    auto handle = Execute([out = std::move(pipe.in)] {
      TimerFd timer;
      timer.wait();
      SEX_ASSERT(write(out.getInt(), "a", 1) == 1);
    }, {});

    std::this_thread::sleep_for(100ms);
    kill(handle.getPid(), SIGKILL);
    auto status = std::move(handle).wait();
    expect(status.isSignaled() && that % status.signal() == SIGKILL);

    char c;
    expect(that % SEX_SYSCALL(read(pipe.out.getInt(), &c, sizeof(c))).unwrap() == 0);
  };

  "not_burning_cpu"_test = [] {
    test::HighResolutionStopwatch stopwatch;
    test::CpuTimeGuard guard(10ms);

    TimerFd t;
    t.set(1ms, 20ms);
    for (int i = 0; i < 20; ++i) {
      expect(that % t.wait() == (size_t)1);
      auto now = stopwatch.TimePassed();
      expect(that % now >= 20ms * i && that % now <= 20ms * i + 21ms);
    }
  };
};
