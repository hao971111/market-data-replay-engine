#include "replay_engine.hpp"
#include "cstdint"
#include "order_sink.hpp"
#include <cstdint>
#include <limits>
#include <iostream>

void ReplayEngine::run(const std::vector<Tick> &ticks, Strategy& strategy, OrderSink& sink, Portfolio &portfolio) {
    int64_t last_timestamp = std::numeric_limits<int64_t>::min();
    int event_index = 0;
    for(const Tick &t : ticks) {
        event_index++;
        if(t.timestamp_us < last_timestamp) {
            std::cout << "Time order violated at event: " << event_index << " last_timestamp: " 
            << last_timestamp << " cur_timestamp: " << t.timestamp_us << std::endl;
            break;
        }   
        last_timestamp = t.timestamp_us;
        portfolio.update_to_market(t.price);
        strategy.on_tick(t, sink);
    }
}