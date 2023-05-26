#pragma once

#include <fmt/format.h>
#include "panic.hpp"

//#define SEX_LIKELY(expr) __builtin_expect((expr), 1)
#define SEX_LIKELY(expr) (expr)
#define SEX_UNLIKELY(expr) !SEX_LIKELY(!(expr))

#define SEX_ASSERT(expr)                                              \
do {                                                                  \
  if (SEX_UNLIKELY(!(expr))) {                                        \
    ::sex::util::Panic(fmt::format("{:s} is not satisfied", #expr));  \
  }                                                                   \
} while (0)