#pragma once
#include <stdint.h>
#include <string>
#include <unordered_map>
#include "order.hpp"
#include "trade.hpp"
class Portfolio {
    double cash_ = 0;
    std::unordered_map<uint32_t, int64_t> position_;
    std::unordered_map<uint32_t, double> last_price_;
public:
    explicit Portfolio(double initial_cash) : cash_(initial_cash) {}
    double get_cash() const {
        return cash_;
    }
    int64_t get_position_by_symbol(uint32_t symbol_id) const {
        auto it = position_.find(symbol_id);
        if(it == position_.end()){
            return  0;
        }
        return it->second;
    }
     double get_last_price_by_symbol(uint32_t symbol_id) const {
        auto it = last_price_.find(symbol_id);
        if(it == last_price_.end()) {
            return 0;
        }
        return it->second;
    }
    void on_trade(const Trade& trade) {
        uint32_t symbol_id = trade.symbol_id;
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
    void update_to_market(uint32_t symbol_id, double price) {
        last_price_[symbol_id] = price;
    }
    double equity() const {
        double holding_price = 0;
        for (const auto& [symbol, price] : position_) {
            auto price_it = last_price_.find(symbol);
            if(price_it != last_price_.end()) {
                holding_price += price*price_it->second;
            }
        }
        return cash_ + holding_price;
    }
};