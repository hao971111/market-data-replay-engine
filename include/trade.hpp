#pragma once
#include <stdint.h>
#include <string>
#include "order.hpp"

struct Trade {
    int64_t timestamp_us;
    std::string symbol;
    SideState side;
    int32_t quantity;
    double price;
};