#include <chrono>
#include <cstdio>
#include <iostream>
#include <vector>
#include "backtest_report.hpp"
#include "market_data_loader.hpp"
#include "order.hpp"
#include "portfolio.hpp"
#include "replay_engine.hpp"
#include "strategy.hpp"
#include "order_sink.hpp"
#include "matching_engine.hpp"
#include "metrics.hpp"

class CountingStrategy : public Strategy {
public:
    int count = 0;
    void on_tick(const Tick& tick, OrderSink& sink) override {
        count++;
        if(count%2 == 0) {
            Order order;
            order.timestamp_us = tick.timestamp_us;
            order.symbol = tick.symbol;
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
int main() {
    Metrics metrics;
    std::vector<Tick> ticks = LoadTicksCsv("data/sample_ticks.csv");
    Portfolio  port(100000);
    MatchingEngine sink(port);
    ReplayEngine re;
    CountingStrategy cs;
    metrics.start_time = std::chrono::steady_clock::now();
    re.run(ticks, cs, sink, port);
    metrics.end_time = std::chrono::steady_clock::now();
    
    auto duration = metrics.end_time - metrics.start_time;
    double seconds = std::chrono::duration<double>(duration).count();
    metrics.ticks_processed = ticks.size();
    double eps = static_cast<double>(metrics.ticks_processed) / seconds;
    
    BacktestReport report;
    report.ticks = ticks.size();
    report.orders = sink.get_orders_received();
    report.trades = sink.size();
    report.initial_cash = 100000;
    report.final_equity = port.equity();
    report.pnl = port.equity() - report.initial_cash;
    report.duration_seconds = seconds;
    report.events_per_sec = eps;

    std::cout << report << std::endl;
}