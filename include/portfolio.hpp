#pragma once
#include <stdint.h>
#include "order.hpp"
#include "trade.hpp"
class Portfolio {
    double cash_ = 0;
    int64_t position_ = 0;
    double last_price_ = 0;
public:
    explicit Portfolio(double initial_cash) : cash_(initial_cash) {}
    double get_cash() const {
        return cash_;
    }
    int64_t get_position() const {
        return position_;
    }
    double get_last_price() const {
        return  last_price_;
    }
    void on_trade(const Trade& trade) {
        switch (trade.side) {
            case SideState::BUY :
                position_ += trade.quantity;
                cash_ -= trade.quantity*trade.price;
                break;
            case SideState::SELL :
                position_ -= trade.quantity;
                cash_ += trade.quantity*trade.price;
                break;
            default:
                break;
        }
    }
    void update_to_market(double price) {
        last_price_ = price;
    }
    double equity() const {
        return cash_ + position_ * last_price_;
    }
};