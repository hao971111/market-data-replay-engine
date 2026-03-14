#include "replay_engine.hpp"
#include "cstdint"
#include "order_sink.hpp"
#include <cstdint>
#include <limits>
#include <iostream>

void ReplayEngine::run(const std::vector<Tick> &ticks, Strategy& strategy, OrderSink& sink, Portfolio &portfolio, ReplayPhaseStats *stats) {
    int64_t last_timestamp = std::numeric_limits<int64_t>::min();
    int event_index = 0;
    for (const Tick &t : ticks) {
        event_index++;
        if (t.timestamp_us < last_timestamp) {
            std::cout << "Time order violated at event: " << event_index << " last_timestamp: "
                      << last_timestamp << " cur_timestamp: " << t.timestamp_us << std::endl;
            break;
        }
        last_timestamp = t.timestamp_us;

        if (stats == nullptr) {
            portfolio.update_to_market(t.symbol_id, t.price);
            strategy.on_tick(t, sink);
        } else {
            stats->events++;
            const int effective_interval = std::min(1000, static_cast<int>(ticks.size()));
            bool do_sample = (event_index % effective_interval == 0);

            if (!do_sample) {
                portfolio.update_to_market(t.symbol_id, t.price);
                strategy.on_tick(t, sink);
            } else {
                stats->sampled_events++;
                auto update_start = std::chrono::steady_clock::now();
                portfolio.update_to_market(t.symbol_id, t.price);
                auto update_end = std::chrono::steady_clock::now();
                stats->market_update_ns += (update_end - update_start);

                auto on_tick_start = std::chrono::steady_clock::now();
                strategy.on_tick(t, sink);
                auto on_tick_end = std::chrono::steady_clock::now();
                stats->strategy_ns += (on_tick_end - on_tick_start);
            }
        }
    }
}