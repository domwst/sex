#pragma once

#include <sex/util/fd_holder.hpp>
#include <sex/detail/syscall.hpp>
#include <sex/detail/execute_hooks.hpp>
#include <sex/detail/process_knob.hpp>
#include <sex/detail/execute_args.hpp>
#include <sex/detail/routine.hpp>

#include <linux/sched.h>
#include <syscall.h>
#include <sys/prctl.h>
#include <csignal>
#include <cstdint>
#include <sched.h>
#include <fstream>

namespace sex {

ProcessKnob Execute(detail::Routine f, ExecuteArgs args, IExecuteHooks& hooks);

ProcessKnob Execute(detail::Routine f, ExecuteArgs args, IExecuteHooks&& hooks = ExecuteHooksDefault{});

}  // namespace sex
