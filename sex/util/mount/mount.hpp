#pragma once

#include "sex/detail/syscall_result.hpp"

namespace sex::util {

struct IMount {
  virtual Result<> mount() = 0;

  virtual Result<> unmount() = 0;

  // Detaches mount from the object, i.e. when the object is destroyed, the mount continues to exist
  virtual void detach() && = 0;

  virtual ~IMount() = default;
};

} // namespace sex::util