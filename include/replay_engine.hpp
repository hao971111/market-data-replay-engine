#pragma once
#include "order_sink.hpp"
#include "strategy.hpp"
#include "tick.hpp"
#include <limits>
#include <iostream>
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

    template <typename StrategyT, typename SinkT>
    void run_fast(const std::vector<Tick>& ticks, StrategyT& strategy, SinkT& sink, Portfolio& portfolio) {
        int64_t last_timestamp = std::numeric_limits<int64_t>::min();
        int event_index = 0;

        for (const Tick& t : ticks) {
            event_index++;
            if (t.timestamp_us < last_timestamp) {
                std::cout << "Time order violated at event: " << event_index
                          << " last_timestamp: " << last_timestamp
                          << " cur_timestamp: " << t.timestamp_us << std::endl;
                break;
            }
            last_timestamp = t.timestamp_us;

            portfolio.update_to_market(t.symbol_id, t.price);
            strategy.on_tick(t, sink); // 这里是静态分发，不走虚调用
        }
    }
};

