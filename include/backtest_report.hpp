#pragma once
#include <chrono>
#include <cstdint>
#include <ostream>
#include <iomanip>

struct BacktestReport {
    std::uint64_t ticks = 0;
    std::uint64_t orders = 0;
    std::uint64_t trades = 0;
    double initial_cash = 0;
    double final_equity = 0;
    double pnl = 0;
    double duration_seconds = 0;
    double ticks_per_sec = 0;
    double orders_per_sec = 0;
    std::chrono::steady_clock::time_point start_time;
    std::chrono::steady_clock::time_point end_time;
};

inline std::ostream& operator<<(std::ostream& os, const BacktestReport& report) {
    os << "ticks: " << report.ticks << "\n";
    os << "orders: " << report.orders << "\n";
    os << "trades: " << report.trades << "\n";
    
    os << "pnl: " << report.pnl << "\n";
    os << std::fixed << std::setprecision(2);
    os << "initial_cash: " << report.initial_cash << "\n";
    os << "final_equity: " << report.final_equity << "\n";
    os << std::defaultfloat;
    os << "duration_seconds: " << report.duration_seconds << "\n";
    os << "ticks_per_sec: " << report.ticks_per_sec  << "\n";
    os << "orders_per_sec : " << report.orders_per_sec;

   
    
    
    return os;
}