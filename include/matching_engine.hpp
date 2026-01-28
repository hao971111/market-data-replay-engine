#pragma once
#include "order_sink.hpp"
#include "trade.hpp"
#include "portfolio.hpp"
#include <vector>

class MatchingEngine : public OrderSink {
    std::vector<Trade> trades;
    Portfolio &portfolio;
public:
    explicit MatchingEngine(Portfolio &port) : portfolio(port){}
    void on_order(const Order& order) override;
    size_t size() {return trades.size();}
};