#pragma once
#include "matching_engine.hpp"
#include "portfolio.hpp"
#include "replay_engine.hpp"
#include "strategy.hpp"
#include "tick.hpp"
#include <cstdint>
#include <vector>
#include <iostream>

class BacktestRunner {
    Portfolio portfolio_;
    MatchingEngine matching_engine_;
    Strategy &strategy_;
    ReplayEngine replay_engine_;
public:
    explicit BacktestRunner(Strategy &strategy) : portfolio_(), matching_engine_(portfolio_), strategy_(strategy), replay_engine_() {}
    void run(const std::vector<Tick>& ticks) {
        int event_index = 0;
        for(const Tick &t : ticks) {
            int64_t last_timestamp = INT64_MIN;
            if(t.timestamp_us < last_timestamp) {
                std::cout << "Time order violated at event: " << event_index << " last_timestamp: " 
                << last_timestamp << " cur_timestamp: " << t.timestamp_us << std::endl;
                break;
            }   
            last_timestamp = t.timestamp_us;
            strategy_.on_tick(t, matching_engine_);
        }
    }
};