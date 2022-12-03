#include "common/cpu_time_guard.hpp"
#include "common/stopwatch/high_resolution_stopwatch.hpp"

#include <sex/util/timer_fd.hpp>
#include <sex/util/pipe.hpp>
#include <sex/execute.hpp>

#include <boost/ut.hpp>

#include <thread>
#include <fcntl.h>

namespace ut = boost::ut;

using Duration = sex::TimerFd::Duration;

size_t GetMs(Duration d) {
  return duration_cast<std::chrono::milliseconds>(d).count();
}

static constexpr Duration time_eps = std::chrono::milliseconds(10);

void ExpectExpireIn(sex::TimerFd& timer, Duration expected,
                    Duration precision = time_eps) {

  using namespace ut;
  using namespace sex;

  auto start = std::chrono::high_resolution_clock::now();
  expect(eq(timer.Wait(), size_t(1)));
  auto end = std::chrono::high_resolution_clock::now();

  auto elapsed = end - start;
  auto deviation = std::chrono::duration_cast<Duration>(abs(elapsed - expected));
  expect(le(deviation.count(), precision.count()));
}

static ut::suite timer_fd = [] {
  using namespace ut;
  using namespace sex;
  using namespace std::chrono_literals;

  "simple_1"_test = [] {
    TimerFd timer;
    timer.Set(200ms);
    ExpectExpireIn(timer, 200ms);
  };

  "simple_2"_test = [] {
    constexpr Duration first_expiration = 200ms;
    constexpr Duration interval = 50ms;

    TimerFd timer;
    timer.Set(first_expiration, interval);
    ExpectExpireIn(timer, first_expiration);
    for (int i = 0; i < 3; ++i) {
      ExpectExpireIn(timer, interval);
    }
    auto spec = timer.Cancel();

    expect(eq(spec.interval.count(), interval.count()));
    expect(le(spec.first_expiration.count(), interval.count()));
    expect(ge(spec.first_expiration.count(), (interval - time_eps).count()));
  };

  "non_blocking"_test = [] {
    constexpr Duration delay = 100ms;

    TimerFd timer(TimerFd::NONBLOCKING);
    timer.Set(delay);
    expect(eq(timer.Wait(), size_t(0)));
    std::this_thread::sleep_for(delay + 10ms);
    expect(eq(timer.Wait(), size_t(1)));
  };

  "blocking_indefinitely"_test = [] {
    auto pipe = sex::util::MakePipe();

    auto handle = Execute([out = std::move(pipe.in)] {
      TimerFd timer;
      timer.Wait();
      SEX_ASSERT(write(out.GetInt(), "a", 1) == 1);
    }, {});

    std::this_thread::sleep_for(100ms);
    kill(handle.Pid(), SIGKILL);
    auto status = std::move(handle).Wait();
    expect(status.IsSignaled() && eq(status.Signal(), SIGKILL));

    char c;
    expect(eq(SEX_SYSCALL(read(pipe.out.GetInt(), &c, sizeof(c))), 0));
  };

  "not_burning_cpu"_test = [] {
    test::HighResolutionStopwatch stopwatch;
    test::CpuTimeGuard guard(10ms);

    TimerFd t;
    t.Set(1ms, 20ms);
    for (int i = 0; i < 20; ++i) {
      expect(eq(t.Wait(), (size_t)1));
      auto now = stopwatch.TimePassed();
      expect(ut::detail::and_(now >= 20ms * i, now <= 20ms * i + 21ms));
    }
  };
};
