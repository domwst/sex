#pragma once

#include <mutex>
#include <atomic>

namespace sex::util {

template<class F>
class Once {
 public:
  Once(F&& f) : action_(std::forward<F>(f)) {
  }

  void operator()() noexcept(noexcept(F{}())) {
    if (invoked_.load(std::memory_order_acquire)) {
      return;
    }
    std::lock_guard lk(mutex_);
    if (invoked_.load(std::memory_order_relaxed)) {
      return;
    }
    action_();
    invoked_.store(true, std::memory_order_release);
  }

 private:
  std::atomic<bool> invoked_{false};
  std::mutex mutex_;
  std::remove_cvref_t<F> action_;
};

}  // namespace sex::util
