#include "market_data_loader.hpp"
#include "tick.hpp"
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

std::vector<Tick> LoadTicksCsv(const std::string& file_name, SymbolTable& symbol_table){

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
            uint32_t symbol_id = symbol_table.get_or_create_id(symbol);
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

bool SaveTicksBin(const std::string& file_name, const std::vector<Tick>& ticks) {
    std::ofstream out(file_name, std::ios::binary);
    if (!out.is_open()) {
        std::cerr << "Failed to open bin file for write: " << file_name << std::endl;
        return false;
    }

    const std::uint64_t n = static_cast<std::uint64_t>(ticks.size());
    out.write(reinterpret_cast<const char*>(&n), sizeof(n));
    if (!out) return false;

    for (const Tick& t : ticks) {
        out.write(reinterpret_cast<const char*>(&t.timestamp_us), sizeof(t.timestamp_us));
        out.write(reinterpret_cast<const char*>(&t.symbol_id), sizeof(t.symbol_id));
        out.write(reinterpret_cast<const char*>(&t.price), sizeof(t.price));
        out.write(reinterpret_cast<const char*>(&t.volume), sizeof(t.volume));
        if (!out) return false;
    }

    return true;
}

std::vector<Tick> LoadTicksBin(const std::string& file_name, SymbolTable& symbol_table) {
    (void)symbol_table;

    std::vector<Tick> ticks;
    std::ifstream in(file_name, std::ios::binary);
    if (!in.is_open()) {
        std::cerr << "Failed to open bin file: " << file_name << std::endl;
        return ticks;
    }

    std::uint64_t n = 0;
    in.read(reinterpret_cast<char*>(&n), sizeof(n));
    if (!in) {
        std::cerr << "Failed to read bin header: " << file_name << std::endl;
        return {};
    }

    ticks.reserve(static_cast<std::size_t>(n));
    for (std::uint64_t i = 0; i < n; ++i) {
        int64_t timestamp_us = 0;
        uint32_t symbol_id = 0;
        double price = 0.0;
        int32_t volume = 0;

        in.read(reinterpret_cast<char*>(&timestamp_us), sizeof(timestamp_us));
        in.read(reinterpret_cast<char*>(&symbol_id), sizeof(symbol_id));
        in.read(reinterpret_cast<char*>(&price), sizeof(price));
        in.read(reinterpret_cast<char*>(&volume), sizeof(volume));

        if (!in) {
            std::cerr << "Corrupted/truncated bin at record " << i
                      << " in " << file_name << std::endl;
            return {};
        }

        ticks.emplace_back(timestamp_us, symbol_id, price, volume);
    }

    return ticks;
}