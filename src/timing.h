#pragma once

#include <chrono>

namespace sft {

using MillisecondsF = std::chrono::duration<double, std::milli>;
using SecondsF = std::chrono::duration<double>;
using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<std::chrono::high_resolution_clock>;

}  // namespace sft
