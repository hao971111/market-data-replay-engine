#pragma once
#include <stdint.h>
#include <string>
struct Tick {
    int64_t timestamp_us;
    std::string symbol;
    double price;
    int32_t volume;
    Tick(int64_t timestamp_us, std::string symbol, double price, int32_t volume) : 
    timestamp_us(timestamp_us), symbol(symbol), price(price), volume(volume) {}
};