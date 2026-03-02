#include "market_data_loader.hpp"
#include "tick.hpp"
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>
std::vector<Tick> LoadTicksCsv(const std::string& file_name){
    std::unordered_map<std::string, uint32_t> symbol_to_id;//TODO: extract the map into SymbolTable class
    uint32_t next_id = 0;

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
    int line_count = 0;
    while(std::getline(file, line)) {
        line_count++;
        std::stringstream ss(line);
        std::getline(ss, timestamp_us_str, ',');
        std::getline(ss, symbol, ',');
        std::getline(ss, price_str, ',');
        std::getline(ss, volume_str, ',');
        if(symbol_to_id.find(symbol) == symbol_to_id.end()) {
            symbol_to_id[symbol] = next_id++;
        }
        uint32_t symbol_id = symbol_to_id[symbol];
        try {
            std::size_t pos = 0;
            int64_t timestamp_us = std::stoll(timestamp_us_str, &pos);
            if (pos != timestamp_us_str.size()) {
                throw std::invalid_argument("");
            }
            pos = 0;
            double price = std::stod(price_str, &pos);
            if (pos != price_str.size()) {
                throw std::invalid_argument("");
            }
            pos = 0;
            int32_t volume = std::stoi(volume_str, &pos);
            if (pos != volume_str.size()) {
                throw std::invalid_argument("");
            }
            ticks.emplace_back(timestamp_us, symbol_id, price, volume);
        } catch (...) {
            if(line.find("timestamp") == std::string::npos &&
               line.find("symbol") == std::string::npos && 
               line.find("price") == std::string::npos && 
               line.find("volume") == std::string::npos) {
                std::cerr << "Invalid line: " << line_count << " " << line << std::endl;
            }
            continue;
        }
    }
    return ticks;
}