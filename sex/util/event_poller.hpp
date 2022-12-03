#pragma once

#include <sex/util/fd_holder.hpp>
#include <sex/detail/syscall.hpp>

#include <span>
#include <vector>

#include <sys/epoll.h>

namespace sex {

class EventPoller : protected FdHolder {
 public:
  using FdHolder::operator FileDescriptor;

  using Cookie = void*;

  enum Flags {
    NONE = 0,
    NO_CLOSE_ON_EXEC = EPOLL_CLOEXEC,
  };

  enum EventMask {
    EMPTY = 0,
    IN = EPOLLIN,
    OUT = EPOLLOUT,
  };

  struct Event {
    EventMask events;
    Cookie cookie;
  };

  explicit EventPoller(Flags flags = NONE)
    : FdHolder(SEX_SYSCALL(epoll_create1(flags ^ NO_CLOSE_ON_EXEC))) {
  }

  void Add(FileDescriptor fd, Cookie cookie, EventMask events = EMPTY) {
    EpollCtl(fd, cookie, events, EPOLL_CTL_ADD);
  }

  void Update(FileDescriptor fd, Cookie cookie, EventMask events = EMPTY) {
    EpollCtl(fd, cookie, events, EPOLL_CTL_MOD);
  }

  void Remove(FileDescriptor fd) {
    EpollCtl(fd, nullptr, EMPTY, EPOLL_CTL_DEL);
  }

  size_t TimedPoll(std::span<Event> events, int millis) {
    const auto MaxSize = events.size();
    std::vector<epoll_event> evs(MaxSize);  // TODO: Get rid of dynamic allocations
    const size_t polled = SEX_SYSCALL(epoll_wait(GetInt(), evs.data(), MaxSize, millis));
    for (size_t i = 0; i < polled; ++i) {
      events[i].events = static_cast<EventMask>(evs[i].events);
      events[i].cookie = evs[i].data.ptr;
    }
    return polled;
  }

  size_t Poll(std::span<Event> events) {
    return TimedPoll(events, -1);
  }

  size_t PollNoWait(std::span<Event> events) {
    return TimedPoll(events, 0);
  }

 private:
  void EpollCtl(FileDescriptor fd, Cookie cookie, EventMask events, int op) {
    auto event = CreateEpollEvent(cookie, events);
    SEX_SYSCALL(epoll_ctl(GetInt(), op, int(fd), &event));
  }

  static epoll_event CreateEpollEvent(Cookie cookie, EventMask flags) {
    return {
      .events = flags,
      .data = epoll_data {
        .ptr = cookie,
      }
    };
  }
};

}  // namespace sex
