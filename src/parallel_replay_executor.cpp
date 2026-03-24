#include "parallel_replay_executor.hpp"

#include "matching_engine.hpp"
#include "order.hpp"
#include "portfolio.hpp"
#include "replay_engine.hpp"

namespace {

class FastCountingStrategy {
public:
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
} // namespace

std::vector<std::vector<Tick>> partition_ticks_by_symbol(const std::vector<Tick>& ticks, int shard_count
) {
    std::vector<std::vector<Tick>> shard_ticks(shard_count);
    for (const Tick& t : ticks) {
        int shard_id = t.symbol_id % shard_count;
        shard_ticks[shard_id].push_back(t);
    }
    return shard_ticks;
}

ParallelReplayResult run_parallel_bench( const std::vector<std::vector<Tick>>& shard_ticks, const ParallelBenchConfig& config
) {
    return run_parallel_replay(
        shard_ticks,
        config.iterations,
        [&](const std::vector<Tick>& shard, int iterations) -> ParallelReplayResult {
            ReplayEngine replay_engine;
            Portfolio portfolio(config.initial_cash);
            portfolio.init_symbol_capacity(config.symbol_count);
            MatchingEngine matching_engine(portfolio);
            FastCountingStrategy strategy;

            ParallelReplayResult result;
            for (int i = 0; i < iterations; ++i) {
                std::uint64_t before = matching_engine.size();  
                replay_engine.run_fast(shard, strategy, matching_engine, portfolio);
                std::uint64_t after = matching_engine.size();
                result.total_ticks += shard.size();  
                result.total_trades += (after - before);
            }

            result.cash = portfolio.get_cash();
            result.positions.resize(config.symbol_count);
            for (std::size_t i = 0; i < config.symbol_count; ++i) {  
                result.positions[i] = portfolio.get_position_by_symbol(static_cast<uint32_t>(i));
            }
            return result;
        }
    );
}