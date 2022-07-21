#pragma once

#include <utility>

namespace sex::detail {

auto less(auto val) {
  using T = std::remove_cvref_t<decltype(val)>;
  return [val](T x) {
    return x < val;
  };
}

auto greater(auto val) {
  using T = std::remove_cvref_t<decltype(val)>;
  return [val](T x) {
    return x > val;
  };
}

auto ne(auto val) {
  using T = std::remove_cvref_t<decltype(val)>;
  return [val](T x) {
    return x != val;
  };
}

auto eq(auto val) {
  using T = std::remove_cvref_t<decltype(val)>;
  return [val](T x) {
    return x == val;
  };
}

}  // namespace sex::detail
