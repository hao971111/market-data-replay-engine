#include <chrono>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <iostream>
#include <vector>
#include <string>
#include "backtest_report.hpp"
#include "market_data_loader.hpp"
#include "order.hpp"
#include "portfolio.hpp"
#include "replay_engine.hpp"
#include "strategy.hpp"
#include "order_sink.hpp"
#include "matching_engine.hpp"
#include "metrics.hpp"

const int N = 100000;
class CountingStrategy : public Strategy {
public:
    int count = 0;
    void on_tick(const Tick& tick, OrderSink& sink) override {
        count++;
        if(count%2 == 0) {
            Order order;
            order.timestamp_us = tick.timestamp_us;
            order.symbol_id = tick.symbol_id;
            order.side = SideState::BUY;
            order.quantity = 1;
            order.price = tick.price;
            sink.on_order(order);
        }
    }
};
class VectorOrderSink : public OrderSink {
    std::vector<Order> orders;
public:
    void on_order(const Order& order) override {
        orders.push_back(order);
    }
    size_t size() {
        return orders.size();
    }
};

int main(int argc, char** argv) {
    std::vector<Tick> ticks = LoadTicksCsv("data/sample_ticks.csv");
    ReplayEngine re;

    // 默认走功能回测；传 --bench 才做性能压测
    const bool bench = (argc >= 2 && std::string(argv[1]) == "--bench");
    const bool bt    = (argc < 2) || (std::string(argv[1]) == "--bt");

    if (!bench && !bt) {
        std::cerr << "usage: replay.exe [--bt | --bench]\n";
        return 1;
    }

    Metrics metrics;
    BacktestReport report;

    if (bench) {
        const int N = 1000000;

        std::uint64_t total_ticks = 0;
        std::uint64_t total_trades = 0;

        metrics.start_time = std::chrono::steady_clock::now();
        for (int i = 0; i < N; ++i) {
            Portfolio port(100000);
            MatchingEngine sink(port);
            CountingStrategy cs;

            re.run(ticks, cs, sink, port);
            total_ticks += ticks.size();
            total_trades += sink.size();
        }
        metrics.end_time = std::chrono::steady_clock::now();

        auto duration = metrics.end_time - metrics.start_time;
        double seconds = std::chrono::duration<double>(duration).count();

        double tps = seconds > 0 ? (static_cast<double>(total_ticks) / seconds) : 0.0;
        double ops = seconds > 0 ? (static_cast<double>(total_trades) / seconds) : 0.0;

        report.ticks = total_ticks;
        report.orders = total_trades;
        report.trades = total_trades;
        report.duration_seconds = seconds;
        report.ticks_per_sec = tps;
        report.orders_per_sec = ops;

        std::cout << report << std::endl;

        std::string filename = "bench_results.csv";
        bool file_exists = std::filesystem::exists(filename);
        std::ofstream file(filename, std::ios::app);
        if (!file_exists) {
            file << "version,timestamp_iso,N,ticks,orders,trades,duration_seconds,ticks_per_sec,orders_per_sec\n";    
        }

        auto now = std::chrono::system_clock::now();
        auto now_time_t = std::chrono::system_clock::to_time_t(now);
        const std::string version = "opt_symbol_id_v1";
        std::stringstream timestamp;
        timestamp << std::put_time(std::localtime(&now_time_t), "%Y-%m-%d %H:%M:%S");
        file << version << "," << timestamp.str() << ","
        << N << ","
        << total_ticks << ","
        << total_trades << ","
        << total_trades << ","
        << std::fixed << std::setprecision(6) << seconds << ","
        << std::fixed << std::setprecision(2) << tps << ","
        << std::fixed << std::setprecision(2) << ops << "\n";
   
        file.close();
        std::cout << "Benchmark results appended to " << filename << std::endl;
        return 0;
    }

    // --bt：只跑 1 次，输出 pnl/equity
    const double initial_cash = 100000.0;
    Portfolio port(initial_cash);
    MatchingEngine sink(port);
    CountingStrategy cs;

    metrics.start_time = std::chrono::steady_clock::now();
    re.run(ticks, cs, sink, port);
    metrics.end_time = std::chrono::steady_clock::now();

    auto duration = metrics.end_time - metrics.start_time;
    double seconds = std::chrono::duration<double>(duration).count();

    const std::uint64_t t = ticks.size();
    const std::uint64_t tr = sink.size();

    double tps = seconds > 0 ? (static_cast<double>(t) / seconds) : 0.0;
    double ops = seconds > 0 ? (static_cast<double>(tr) / seconds) : 0.0;

    report.ticks = t;
    report.orders = tr;   // 现在你的模型里 order==trade
    report.trades = tr;
    report.initial_cash = initial_cash;
    report.final_equity = port.equity();
    report.pnl = report.final_equity - report.initial_cash;
    report.duration_seconds = seconds;
    report.ticks_per_sec = tps;
    report.orders_per_sec = ops;

    std::cout << report << std::endl;
    return 0;
}