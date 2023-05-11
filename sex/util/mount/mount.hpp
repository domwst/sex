#pragma once

#include "sex/util/result.hpp"

namespace sex::util {

struct IMount {
  virtual Result<> mount() = 0;

  virtual Result<> unmount() = 0;

  // Detaches mount from the object, i.e. when the object is destroyed, the mount continues to exist
  virtual void detach() && = 0;

  virtual ~IMount() = default;
};

} // namespace sex::util