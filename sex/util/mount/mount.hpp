#pragma once

#include "sex/detail/syscall_result.hpp"

namespace sex::util {

struct IMount {
  virtual Result<> mount() = 0;

  virtual Result<> unmount() = 0;

  virtual ~IMount() = default;
};

} // namespace sex::util