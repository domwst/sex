#pragma once

#include <mutex>
#include <atomic>

namespace sex::util {

class Once {
 public:
  template<class F>
  void Do(F&& f) noexcept(noexcept(F{}())) {
    if (invoked_.load(std::memory_order_acquire)) {
      return;
    }
    std::lock_guard lk(mutex_);
    if (invoked_.load(std::memory_order_relaxed)) {
      return;
    }
    f();
    invoked_.store(true, std::memory_order_release);
  }

 private:
  std::atomic<bool> invoked_{false};
  std::mutex mutex_;
};

}  // namespace sex::util
