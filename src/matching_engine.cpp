#include "matching_engine.hpp"

void MatchingEngine::on_order(const Order& order) {
    Trade trade;
    trade.timestamp_us = order.timestamp_us;
    trade.symbol = order.symbol;
    trade.side = order.side;
    trade.price = order.price;
    trade.quantity = order.quantity;
    trades.push_back(trade);
}