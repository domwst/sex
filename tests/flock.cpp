#include <sex/util/flock.hpp>
#include <sex/detail/syscall.hpp>

#include <boost/ut.hpp>

#include <atomic>
#include <chrono>
#include <iostream>
#include <random>
#include <vector>
#include <thread>

using namespace sex;
using namespace boost::ut;
using namespace std::chrono_literals;

static suite file_lock_guard = [] {
  "flock_allows_several_readers"_test = [] {
    FdHolder fd(SEX_SYSCALL(open("test", O_RDONLY | O_CREAT | O_EXCL, 0644)).unwrap());

    std::atomic<uint> rcnt{0};

    std::vector<std::thread> workers;
    workers.reserve(10);

    for (int i = 0; i < 10; ++i) {
      workers.emplace_back([&rcnt] {
        for (int i = 0; i < 200; ++i) {
          util::FileLockGuard lk("test", util::FileLockGuard::RLock);

          rcnt.fetch_add(1, std::memory_order_relaxed);
          std::this_thread::sleep_for(1ms);
          rcnt.fetch_sub(1, std::memory_order_relaxed);
        }
      });
    }

    for (auto& w : workers) {
      w.join();
    }

    SEX_SYSCALL(remove("test")).ensure();
  };

  "write_lock_excludes_readers"_test = [] {
    FdHolder(SEX_SYSCALL(open("test", O_RDONLY | O_EXCL | O_CREAT, 0644)).unwrap());

    std::atomic<uint> rcount{0};
    std::atomic<uint> wcount{0};

    std::vector<std::thread> workers;
    workers.reserve(10);

    for (int i = 0; i < 10; ++i) {
      workers.emplace_back([&rcount, &wcount, id = i] {
        std::mt19937 rnd(id);
        for (int i = 0; i < 300; ++i) {
          if (rnd() & 7) {
            util::FileLockGuard lk("test", util::FileLockGuard::RWLock);

            expect(that % wcount.fetch_add(1, std::memory_order_relaxed) == 0u);
            expect(that % rcount.load(std::memory_order_relaxed) == 0u);
            std::this_thread::sleep_for(100us);
            expect(that % wcount.fetch_sub(1, std::memory_order_relaxed) == 1u);
          } else {
            util::FileLockGuard lk("test", util::FileLockGuard::RLock);

            rcount.fetch_add(1, std::memory_order_relaxed);
            std::this_thread::sleep_for(1ms);
            rcount.fetch_sub(1, std::memory_order_relaxed);
          }
        }
      });
    }

    for (auto& w : workers) {
      w.join();
    }

    SEX_SYSCALL(remove("test")).ensure();
  };
};