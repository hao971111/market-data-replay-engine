#pragma once
#include "order_sink.hpp"
#include "strategy.hpp"
#include "tick.hpp"
#include <vector>

class ReplayEngine{
public:
    void run(const std::vector<Tick> &ticks, Strategy&  strategy, OrderSink& sink);
};