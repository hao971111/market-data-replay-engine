#pragma once
#include "order_sink.hpp"
#include "tick.hpp"

class Strategy {
public:
    virtual void on_tick(const Tick& tick, OrderSink& sink) = 0;
};