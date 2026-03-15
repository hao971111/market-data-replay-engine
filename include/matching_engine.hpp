#pragma once
#include "order_sink.hpp"
#include "portfolio.hpp"
#include <cstdint>

class MatchingEngine : public OrderSink {
    std::uint64_t orders_received;
    uint64_t trades_count = 0;
    Portfolio &portfolio;
public:
    explicit MatchingEngine(Portfolio &port) : portfolio(port){
        orders_received = 0;
    }
    void on_order(const Order& order) override {
        portfolio.on_order_fill(order);
        trades_count++;
        orders_received++;
    }
    size_t size() {return trades_count;}
    std::uint64_t get_orders_received() {
        return orders_received;
    }
};