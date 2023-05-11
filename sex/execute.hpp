#pragma once

#include <sex/util/fd_holder.hpp>
#include <sex/detail/syscall.hpp>
#include "sex/util/execute_hooks.hpp"
#include "sex/detail/process_knob.hpp"
#include "sex/util/execute_args.hpp"
#include <sex/detail/routine.hpp>

namespace sex {

detail::ProcessKnob Execute(detail::Routine f,
                            util::ExecuteArgs args,
                            util::IExecuteHooks& hooks);

detail::ProcessKnob Execute(detail::Routine f,
                            util::ExecuteArgs args = util::ExecuteArgs{},
                            util:: IExecuteHooks&& hooks = util::ExecuteHooksDefault{});

}  // namespace sex
