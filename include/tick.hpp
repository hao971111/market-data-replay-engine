#pragma once
#include <stdint.h>
struct Tick {
    int64_t timestamp_us;
    uint32_t symbol_id;
    double price;
    int32_t volume;
    Tick(int64_t timestamp_us, uint32_t symbol_id, double price, int32_t volume) : 
    timestamp_us(timestamp_us), symbol_id(symbol_id), price(price), volume(volume) {}
};