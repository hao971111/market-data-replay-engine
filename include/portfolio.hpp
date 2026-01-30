#pragma once
#include <stdint.h>
#include "order.hpp"
#include "trade.hpp"
class Portfolio {
    double cash = 0;
    int64_t position = 0;
    double last_price = 0;
public:
    double get_cash() const {
        return cash;
    }
    int64_t get_position() const {
        return position;
    }
    double get_last_price() const {
        return  last_price;
    }
    void on_trade(const Trade& trade) {
        switch (trade.side) {
            case SideState::BUY :
                position += trade.quantity;
                cash -= trade.quantity*trade.price;
                break;
            case SideState::SELL :
                position -= trade.quantity;
                cash += trade.quantity*trade.price;
                break;
            default:
                break;
        }
    }
    void update_to_market(double price) {
        last_price = price;
    }
    double equity() const {
        return cash + position * last_price;
    }
};