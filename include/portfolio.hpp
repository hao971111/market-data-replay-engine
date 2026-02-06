#pragma once
#include <stdint.h>
#include <string>
#include <unordered_map>
#include "order.hpp"
#include "trade.hpp"
class Portfolio {
    double cash_ = 0;
    std::unordered_map<std::string, int64_t> position_;
    std::unordered_map<std::string, double> last_price_;
public:
    explicit Portfolio(double initial_cash) : cash_(initial_cash) {}
    double get_cash() const {
        return cash_;
    }
    int64_t get_position_by_symbol(const std::string& symbol) const {
        auto it = position_.find(symbol);
        if(it == position_.end()){
            return  0;
        }
        return it->second;
    }
     double get_last_price_by_symbol(const std::string& symbol) const {
        auto it = last_price_.find(symbol);
        if(it == last_price_.end()) {
            return 0;
        }
        return it->second;
    }
    void on_trade(const Trade& trade) {
        std::string symbol = trade.symbol;
        switch (trade.side) {
            case SideState::BUY :
                position_[symbol] += trade.quantity;
                cash_ -= trade.quantity*trade.price;
                break;
            case SideState::SELL :
                position_[symbol] -= trade.quantity;
                cash_ += trade.quantity*trade.price;
                break;
            default:
                break;
        }
    }
    void update_to_market(const std::string& symbol, double price) {
        last_price_[symbol] = price;
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