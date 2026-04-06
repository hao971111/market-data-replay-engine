#include "parallel_replay_executor.hpp"

std::vector<std::vector<Tick>> partition_ticks_by_symbol(const std::vector<Tick>& ticks,
                                                          int shard_count) {
    std::vector<std::vector<Tick>> shard_ticks(shard_count);
    for (const Tick& t : ticks) {
        int shard_id = t.symbol_id % shard_count;
        shard_ticks[shard_id].push_back(t);
    }
    return shard_ticks;
}
