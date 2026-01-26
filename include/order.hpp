#pragma once
#include <stdint.h>
#include <string>
enum class SideState {
    BUY,
    SELL,
};
struct Order{
    int64_t timestamp_us;
    std::string symbol;
    SideState side;
    int32_t quantity;
    double price;
};