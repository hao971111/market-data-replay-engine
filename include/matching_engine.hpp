#pragma once
#include "order_sink.hpp"
#include "trade.hpp"
#include <vector>

class MatchingEngine : public OrderSink {
    std::vector<Trade> trades;
public:
    void on_order(const Order& order) override;
    size_t size() {return trades.size();}
};