#include <boost/ut.hpp>

#include <sex/execute.hpp>
#include <sex/util/pipe.hpp>

#include <unistd.h>

namespace ut = boost::ut;

static ut::suite execute = [] {
  using namespace ut::literals;
  using namespace ut;

  "not_the_same_process"_test = [] {
    int cur_pid = getpid();

    auto handle = sex::Execute([cur_pid]() {
      expect(neq(cur_pid, getpid()));
    }, {});

    auto status = std::move(handle).wait();
    expect(status == sex::util::ExitStatus::Exited(0));
  };

  "proper_exit_code"_test = [] {
    auto handle = sex::Execute([] {
      _exit(12);
    }, {});

    auto status = std::move(handle).wait();
    expect(status == sex::util::ExitStatus::Exited(12));
  };

  "signal_suicide"_test = [] {
    auto handle = sex::Execute([] {
      SEX_SYSCALL(kill(getpid(), SIGTERM)).ensure();
    }, {});

    auto status = std::move(handle).wait();
    expect(status == sex::util::ExitStatus::Signaled(SIGTERM));
  };

  "new_pid_ns"_test = [] {
    auto handle = sex::Execute([] {
      expect(eq(getpid(), 1));
    }, sex::util::ExecuteArgs{}.NewPidNS().NewUserNS());

    auto status = std::move(handle).wait();
    expect(status == sex::util::ExitStatus::Exited(0));
  };

  "correct_pid_in_knob"_test = [] {
    auto pipe = sex::util::MakePipe();

    auto handle = sex::Execute([in = std::move(pipe.in)] {
      int pid = getpid();
      SEX_ASSERT(write(in.getInt(), &pid, sizeof(int)) == sizeof(int));
    }, {});

    int real_child_pid;
    SEX_ASSERT(read(pipe.out.getInt(), &real_child_pid, sizeof(int)) == sizeof(int));

    expect(eq(real_child_pid, handle.getPid()));

    std::move(handle).wait();
  };

  "cant_kill_init"_test = [] {
    auto status = sex::Execute([] {
      SEX_ASSERT(getpid() == 1);
      auto status = sex::Execute([] {
        SEX_ASSERT(kill(1, SIGKILL) == 0);
      }).wait();
      SEX_ASSERT(status == sex::util::ExitStatus::Exited(0));
    }, sex::util::ExecuteArgs{}.NewPidNS().NewUserNS()).wait();
    SEX_ASSERT(status == sex::util::ExitStatus::Exited(0));
  };
};
