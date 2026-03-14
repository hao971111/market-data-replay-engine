#include "matching_engine.hpp"

void MatchingEngine::on_order(const Order& order) {
    portfolio.on_order_fill(order);
    trades_count++;
    orders_received++;
}