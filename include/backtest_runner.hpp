#pragma once
#include "matching_engine.hpp"
#include "portfolio.hpp"
#include "replay_engine.hpp"
#include "strategy.hpp"
#include "tick.hpp"
#include <vector>

class BacktestRunner {
    Portfolio portfolio_;
    MatchingEngine matching_engine_;
    Strategy &strategy_;
    ReplayEngine replay_engine_;
public:
    explicit BacktestRunner(Strategy &strategy) : portfolio_(), matching_engine_(portfolio_), strategy_(strategy), replay_engine_() {}
    void run(const std::vector<Tick>& ticks) {
        replay_engine_.run(ticks, strategy_, matching_engine_);
    }
};