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
};