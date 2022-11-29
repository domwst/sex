#include <boost/ut.hpp>

#include <sex/execute.hpp>
#include <sex/util/pipe.hpp>

#include <unistd.h>

namespace ut = boost::ut;

static bool ExitedWith(const sex::ExitStatus& status, int exit_code) {
  return status.IsExited() && status.ExitCode() == exit_code;
}

static bool ExitedWithZero(const sex::ExitStatus& status) {
  return ExitedWith(status, 0);
}

static bool KilledWith(const sex::ExitStatus& status, int signal) {
  return status.IsSignaled() && status.Signal() == signal;
}

static ut::suite execute = [] {
  using namespace ut::literals;
  using namespace ut;

  "not_the_same_process"_test = [] {
    int cur_pid = getpid();

    auto handle = sex::Execute([cur_pid]() {
      expect(neq(cur_pid, getpid()));
    }, {});

    auto status = std::move(handle).Wait();
    expect(ExitedWithZero(status));
  };

  "proper_exit_code"_test = [] {
    auto handle = sex::Execute([] {
      _exit(12);
    }, {});

    auto status = std::move(handle).Wait();
    expect(ExitedWith(status, 12));
  };

  "signal_suicide"_test = [] {
    auto handle = sex::Execute([] {
      SEX_SYSCALL(kill(getpid(), SIGTERM));
    }, {});

    auto status = std::move(handle).Wait();
    expect(KilledWith(status, SIGTERM));
  };

  "new_pid_ns"_test = [] {
    auto handle = sex::Execute([] {
      expect(eq(getpid(), 1));
    }, sex::ExecuteArgs{}.NewPidNS().NewUserNS());

    auto status = std::move(handle).Wait();
    expect(ExitedWithZero(status));
  };

  "correct_pid_in_knob"_test = [] {
    auto pipe = sex::util::MakePipe();

    auto handle = sex::Execute([in = std::move(pipe.in)] {
      int pid = getpid();
      SEX_ASSERT(write(in.GetInt(), &pid, sizeof(int)) == sizeof(int));
    }, {});

    int real_child_pid;
    SEX_ASSERT(read(pipe.out.GetInt(), &real_child_pid, sizeof(int)) == sizeof(int));

    expect(eq(real_child_pid, handle.Pid()));

    std::move(handle).Wait();
  };
};
