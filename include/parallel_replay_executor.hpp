#pragma once

#include "matching_engine.hpp"
#include "order.hpp"
#include "portfolio.hpp"
#include "replay_engine.hpp"
#include "tick.hpp"
#include <cstdint>
#include <thread>
#include <vector>

struct ParallelReplayResult {
    std::uint64_t total_ticks = 0;
    std::uint64_t total_trades = 0;
    double cash = 0.0;
    std::vector<int64_t> positions;
};

struct ParallelBenchConfig {
    int iterations = 1;
    std::size_t symbol_count = 0;
    double initial_cash = 100000.0;
};

template <typename ShardRunner>
ParallelReplayResult run_parallel_replay(
    const std::vector<std::vector<Tick>>& shard_ticks,
    int iterations,
    ShardRunner&& run_shard
) {
    std::vector<std::thread> threads;
    std::vector<ParallelReplayResult> shard_results(shard_ticks.size());

    for (std::size_t s = 0; s < shard_ticks.size(); ++s) {
        threads.emplace_back([&, s]() {
            shard_results[s] = run_shard(shard_ticks[s], iterations);
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    ParallelReplayResult result;
    for (const auto& r : shard_results) {
      result.total_ticks += r.total_ticks;
      result.total_trades += r.total_trades;
      result.cash += r.cash;
      if (result.positions.empty()) {
          result.positions.resize(r.positions.size(), 0);
      }
      for (std::size_t i = 0; i < r.positions.size(); ++i) {
          result.positions[i] += r.positions[i];
      }
    }
    return result;
}

std::vector<std::vector<Tick>> partition_ticks_by_symbol(
    const std::vector<Tick>& ticks,
    int shard_count
);

/** 压测默认策略；与 main --bench 一致，保持 run_fast 静态分派、无虚调用。 */
namespace bench_strategies {

struct FastCountingStrategy {
    int count = 0;
    void on_tick(const Tick& tick, MatchingEngine& sink) {
        count++;
        if (count % 2 == 0) {
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

}  // namespace bench_strategies

/**
 * 并行分片回放 + 汇总。StrategyT 须具备 void on_tick(const Tick&, MatchingEngine&)，
 * 由模板传入 replay_engine.run_fast，避免 Strategy 虚接口。
 */
template <typename StrategyT>
ParallelReplayResult run_parallel_bench(
    const std::vector<std::vector<Tick>>& shard_ticks,
    const ParallelBenchConfig& config
) {
    return run_parallel_replay(
        shard_ticks,
        config.iterations,
        [&](const std::vector<Tick>& shard, int iterations) -> ParallelReplayResult {
            ReplayEngine replay_engine;
            Portfolio portfolio(config.initial_cash);
            portfolio.init_symbol_capacity(config.symbol_count);
            MatchingEngine matching_engine(portfolio);
            StrategyT strategy;

            ParallelReplayResult result;
            for (int i = 0; i < iterations; ++i) {
                const std::uint64_t before = matching_engine.size();
                replay_engine.run_fast(shard, strategy, matching_engine, portfolio);
                const std::uint64_t after = matching_engine.size();
                result.total_ticks += shard.size();
                result.total_trades += (after - before);
            }

            result.cash = portfolio.get_cash();
            result.positions.resize(config.symbol_count);
            for (std::size_t i = 0; i < config.symbol_count; ++i) {
                result.positions[i] =
                    portfolio.get_position_by_symbol(static_cast<uint32_t>(i));
            }
            return result;
        }
    );
}