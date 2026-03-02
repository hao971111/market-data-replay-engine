#pragma once
#include <stdint.h>
#include <string>
enum class SideState {
    BUY,
    SELL,
};
struct Order{
    int64_t timestamp_us;
    uint32_t symbol_id;
    SideState side;
    int32_t quantity;
    double price;
};