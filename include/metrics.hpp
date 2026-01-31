#include <cstdint>
#include <chrono>

struct Metrics {
    std::uint64_t ticks_processed;
    std::uint64_t orders_processed;
    std::uint64_t trades_executed;
    std::chrono::steady_clock::time_point start_time;
    std::chrono::steady_clock::time_point end_time;
};