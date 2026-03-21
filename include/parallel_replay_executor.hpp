#pragma once

#include "tick.hpp"
#include <cstdint>
#include <vector>
#include <thread>

struct ParallelReplayResult {
    std::uint64_t total_ticks = 0;
    std::uint64_t total_trades = 0;
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
    }
    return result;
}

std::vector<std::vector<Tick>> partition_ticks_by_symbol(
    const std::vector<Tick>& ticks,
    int shard_count
);

ParallelReplayResult run_parallel_bench(
    const std::vector<std::vector<Tick>>& shard_ticks,
    const ParallelBenchConfig& config
);