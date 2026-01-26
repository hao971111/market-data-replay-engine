#include <iostream>
#include <vector>
#include "market_data_loader.hpp"
#include "order.hpp"
#include "replay_engine.hpp"
#include "strategy.hpp"
#include "order_sink.hpp"

class CountingStrategy : public Strategy {
public:
    int count = 0;
    void on_tick(const Tick& tick, OrderSink& sink) override {
        count++;
    }
};
class VectorOrderSink : public OrderSink {
    std::vector<Order> orders;
public:
    void on_order(const Order& order) override {
        orders.push_back(order);
    }
};
int main() {
    std::vector<Tick> ticks = LoadTicksCsv("data/sample_ticks.csv");
    VectorOrderSink sink;
    ReplayEngine re;
    CountingStrategy cs;
    re.run(ticks, cs, sink);
    std::cout << "count: " << cs.count << std::endl;
    std::cout << "size: " << ticks.size() << std::endl;
    return 0;
}