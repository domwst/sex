#pragma once

#include "basic_stopwatch.hpp"

#include <chrono>

namespace sex::test {

using HighResolutionStopwatch = BasicStopwatch<std::chrono::high_resolution_clock>;

}
