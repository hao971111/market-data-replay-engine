#pragma once
#include <cstddef>
#include <stdint.h>
#include <vector>
#include "trade.hpp"
class Portfolio {
    double cash_ = 0;
    std::vector<int64_t> position_;
    std::vector<double> last_price_;
public:
    explicit Portfolio(double initial_cash) : cash_(initial_cash) {}
    
    void init_symbol_capacity(std::size_t count) {
        position_.resize(count);
        last_price_.resize(count);
    }

    void ensure_symbol_capacity(uint32_t symbol_id) {
        if(symbol_id >= position_.size()) {
            position_.resize(symbol_id + 1);
            last_price_.resize(symbol_id + 1);
        }
    }
    double get_cash() const {
        return cash_;
    }
    int64_t get_position_by_symbol(uint32_t symbol_id) const {
        if(symbol_id >= position_.size()) {
            return 0;
        }
        return position_[symbol_id];
    }
     double get_last_price_by_symbol(uint32_t symbol_id) const {
        if(symbol_id >= last_price_.size()) {
            return 0;
        }
        return last_price_[symbol_id];
    }
    void on_trade(const Trade& trade) {
        uint32_t symbol_id = trade.symbol_id;
        ensure_symbol_capacity(symbol_id);
        switch (trade.side) {
            case SideState::BUY :
                position_[symbol_id] += trade.quantity;
                cash_ -= trade.quantity*trade.price;
                break;
            case SideState::SELL :
                position_[symbol_id] -= trade.quantity;
                cash_ += trade.quantity*trade.price;
                break;
            default:
                break;
        }
    }
    void on_order_fill(const Order& order) {
        ensure_symbol_capacity(order.symbol_id);
        switch (order.side) {
            case SideState::BUY:
                position_[order.symbol_id] += order.quantity;
                cash_ -= order.quantity * order.price;
                break;
            case SideState::SELL:
                position_[order.symbol_id] -= order.quantity;
                cash_ += order.quantity * order.price;
                break;
            default:
                break;
        }
    }
    void update_to_market(uint32_t symbol_id, double price) {
        last_price_[symbol_id] = price;
    }
    double equity() const {
        double holding_price = 0;
        for(std::size_t symbol_id = 0 ; symbol_id < position_.size(); ++symbol_id) {
            if(!position_[symbol_id]) {
                continue;
            }
            holding_price += position_[symbol_id]*last_price_[symbol_id];
        }
        return cash_ + holding_price;
    }
};