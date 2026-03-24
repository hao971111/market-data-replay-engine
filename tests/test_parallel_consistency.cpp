#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>
#include <vector>

#include "matching_engine.hpp"
#include "order.hpp"
#include "parallel_replay_executor.hpp"
#include "portfolio.hpp"
#include "replay_engine.hpp"
#include "tick.hpp"

namespace {

class TestStrategy {
public:
    void on_tick(const Tick& tick, MatchingEngine& sink) {
        if ((tick.symbol_id % 2) == 0) {
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

struct RunResult {
    std::uint64_t trades = 0;
    double cash = 0.0;
    std::vector<int64_t> positions;
};

std::vector<Tick> make_test_ticks() {
    return {
        Tick(100, 0, 100.0, 10),
        Tick(101, 1, 101.0, 10),
        Tick(102, 2, 102.0, 10),
        Tick(103, 3, 103.0, 10),
        Tick(104, 0, 100.5, 10),
        Tick(105, 1, 101.5, 10),
        Tick(106, 2, 102.5, 10),
        Tick(107, 3, 103.5, 10),
    };
}

std::size_t compute_symbol_count(const std::vector<Tick>& ticks) {
    uint32_t max_id = 0;
    for (const Tick& t : ticks) {
        if (t.symbol_id > max_id) {
            max_id = t.symbol_id;
        }
    }
    return static_cast<std::size_t>(max_id) + 1;
}

RunResult run_single_thread_result(const std::vector<Tick>& ticks, std::size_t symbol_count) {
    ReplayEngine replay_engine;
    Portfolio portfolio(0.0);
    portfolio.init_symbol_capacity(symbol_count);
    MatchingEngine matching_engine(portfolio);
    TestStrategy strategy;

    replay_engine.run_fast(ticks, strategy, matching_engine, portfolio);

    RunResult result;
    result.trades = matching_engine.size();
    result.cash = portfolio.get_cash();
    result.positions.resize(symbol_count);

    for (std::size_t i = 0; i < symbol_count; ++i) {
        result.positions[i] = portfolio.get_position_by_symbol(static_cast<uint32_t>(i));
    }

    return result;
}

}  // namespace

TEST(parallel_consistency, DISABLED_trade_count_matches_single_thread) {
    const std::vector<Tick> ticks = make_test_ticks();
    const std::size_t symbol_count = compute_symbol_count(ticks);

    const RunResult single_result = run_single_thread_result(ticks, symbol_count);
    
    for (int shard_count : {1, 2, 4}) {
        auto shard_ticks = partition_ticks_by_symbol(ticks, shard_count);

        ParallelBenchConfig config;
        config.iterations = 1;
        config.symbol_count = symbol_count;
        config.initial_cash = 0.0;

        ParallelReplayResult result = run_parallel_bench(shard_ticks, config);

        EXPECT_EQ(result.total_ticks, ticks.size());
        EXPECT_EQ(result.total_trades, single_result.trades);
        EXPECT_DOUBLE_EQ(result.cash, single_result.cash);

        for (std::size_t i = 0; i < symbol_count; ++i) {
            EXPECT_EQ(result.positions[i], single_result.positions[i]);
        }
    }
}