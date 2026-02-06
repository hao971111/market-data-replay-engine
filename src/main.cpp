#include <chrono>
#include <iostream>
#include <vector>
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
    std::cout << "count: " << cs.count << std::endl;
    std::cout << "tick size: " << ticks.size() << std::endl;
    std::cout << "trades size: " << sink.size() << std::endl;
    std::cout << "port cash: " << port.get_cash() << std::endl;
    std::cout << "port equity: " << port.equity() << std::endl;
    std::cout << "pnl: " << port.equity() - 100000 << std::endl;
    auto duration = metrics.end_time - metrics.start_time;
    double seconds = std::chrono::duration<double>(duration).count();
    std::cout << "duration: " << seconds << "s"  <<std::endl;
    metrics.ticks_processed = ticks.size();
    metrics.trades_executed = sink.size();
    double eps = static_cast<double>(metrics.ticks_processed) / seconds;
    std::cout << "events_per_sec: " << eps << "\n";
    std::cout << "TEST position: " << port.get_position_by_symbol("TEST") << std::endl;
    std::cout << "TEST last_price: " << port.get_last_price_by_symbol("TEST") << std::endl;
    std::cout << "ABC position: " << port.get_position_by_symbol("ABC") << std::endl;
    std::cout << "ABC last_price: " << port.get_last_price_by_symbol("ABC") << std::endl;
}