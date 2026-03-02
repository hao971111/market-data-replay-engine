#include "matching_engine.hpp"

void MatchingEngine::on_order(const Order& order) {
    Trade trade;
    trade.timestamp_us = order.timestamp_us;
    trade.symbol_id = order.symbol_id;
    trade.side = order.side;
    trade.price = order.price;
    trade.quantity = order.quantity;
    trades.push_back(trade);
    portfolio.on_trade(trade);
    orders_received++;
}