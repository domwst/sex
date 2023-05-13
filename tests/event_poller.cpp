#include "common/time_print.hpp"
#include "common/stopwatch/high_resolution_stopwatch.hpp"
#include "common/cpu_time_guard.hpp"

#include <boost/ut.hpp>

#include <sex/util/event_poller.hpp>
#include <sex/util/timer_fd.hpp>

#include <chrono>

namespace ut = boost::ut;

static ut::suite event_poller = [] {
  using namespace ut::literals;
  using namespace ut;

  using namespace sex;

  using namespace std::chrono_literals;

  "simple"_test = [] {
    test::HighResolutionStopwatch stopwatch;
    test::CpuTimeGuard guard(5ms);
    util::EventPoller poller;

    util::TimerFd t;
    t.set({.first_expiration = 40ms, .interval = 20ms});
    poller.Add(t, (void*)1, util::EventPoller::IN);

    util::EventPoller::Event event {};
    expect(that % poller.Poll({&event, 1}) == (size_t)1);
    expect(that % event.events == util::EventPoller::IN && that % event.cookie == (void*)1);
    expect(that % t.wait() == (size_t)1);
    expect(that % stopwatch.TimePassed() >= 40ms && that % stopwatch.TimePassed() <= 50ms);

    expect(that % poller.Poll({&event, 1}) == (size_t)1);
    expect(that % event.events == util::EventPoller::IN && that % event.cookie == (void*)1);
    expect(that % t.wait() == (size_t)1);
    expect(that % stopwatch.TimePassed() >= 60ms && that % stopwatch.TimePassed() <= 70ms);
  };

  "order"_test = [] {
    test::CpuTimeGuard guard(5ms);

    util::EventPoller poller;

    std::array<util::TimerFd, 4> timers;
    timers[0].set({.first_expiration = 20ms, .interval = 20ms});
    timers[1].set({.first_expiration = 5ms,  .interval = 20ms});
    timers[2].set({.first_expiration = 1ns,  .interval = 15ms});

    for (size_t i = 0; i < 4; ++i) {
      poller.Add(timers[i], (void*)i, util::EventPoller::IN);
    }

    std::vector<size_t> order;
    for (size_t i = 0; i < 7; ++i) {
      std::array<util::EventPoller::Event, 2> events{};
      expect(that % poller.Poll(events) == (size_t)1);
      auto event = events[0];
      expect(that % event.events == util::EventPoller::IN);
      auto id = (size_t)event.cookie;
      order.push_back(id);
      expect(that % timers[id].wait() == (size_t)1);
    }
    expect(that % order == std::vector<size_t>{2, 1, 2, 0, 1, 2, 0});
  };
};
