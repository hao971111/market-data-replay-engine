#pragma once
#include "order.hpp"
class OrderSink {
public:
    virtual void on_order(const Order&) = 0;
};