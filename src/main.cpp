#include <iostream>
#include <vector>
#include "market_data_loader.hpp"
#include "order.hpp"
#include "portfolio.hpp"
#include "replay_engine.hpp"
#include "strategy.hpp"
#include "order_sink.hpp"
#include "matching_engine.hpp"

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
    std::vector<Tick> ticks = LoadTicksCsv("data/sample_ticks.csv");
    Portfolio  port;
    MatchingEngine sink(port);
    ReplayEngine re;
    CountingStrategy cs;
    re.run(ticks, cs, sink);
    std::cout << "count: " << cs.count << std::endl;
    std::cout << "tick size: " << ticks.size() << std::endl;
    std::cout << "sink size: " << sink.size() << std::endl;
    std::cout << "port cash: " << port.get_cash() << std::endl;
    std::cout << "port postion: " << port.get_position() << std::endl;
    std::cout << "port last_price: " << port.get_last_price() << std::endl;
    std::cout << "port equity: " << port.equity() << std::endl;
    return 0;
}