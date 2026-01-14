#include "market_data_loader.hpp"
#include "tick.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
std::vector<Tick> LoadTicksCsv(const std::string& file_name){
    std::vector<Tick> ticks;
    std::ifstream file(file_name);
    if(!file.is_open()){
        std::cout << "Failed to open file: " << file_name << std::endl;
        return ticks;
    }
    std::string line;
    std::string timestamp_us_str;
    std::string symbol;
    std::string price_str;
    std::string volume_str;
    while(std::getline(file, line)) {
        std::stringstream ss(line);
        std::getline(ss, timestamp_us_str, ',');
        std::getline(ss, symbol, ',');
        std::getline(ss, price_str, ',');
        std::getline(ss, volume_str, ',');

        int64_t timestamp_us = std::stoll(timestamp_us_str);
        double price = std::stod(price_str);
        int32_t volume = std::stoi(volume_str);

        ticks.emplace_back(timestamp_us, symbol, price, volume);
    }
    return ticks;
}