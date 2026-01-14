#include <iostream>
#include "market_data_loader.hpp"
int main() {
    std::vector<Tick> ticks = LoadTicksCsv("data/sample_ticks.csv");
    std::cout << "size: " << ticks.size() << std::endl;
    return 0;
}