#pragma once
#include "order_sink.hpp"
#include "strategy.hpp"
#include "tick.hpp"
#include <cstddef>
#include <cstdint>
#include <vector>
#include <chrono>
#include "portfolio.hpp"

struct ReplayPhaseStats {
    uint64_t sampled_events = 0;
    uint64_t events = 0;
    std::chrono::nanoseconds market_update_ns{0};
    std::chrono::nanoseconds strategy_ns{0};
};
class ReplayEngine{
public:
    void run(const std::vector<Tick> &ticks, Strategy&  strategy, OrderSink& sink, Portfolio &portfolio, ReplayPhaseStats* stats = nullptr);
};

