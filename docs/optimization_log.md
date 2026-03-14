## 2026-03-02 - Opt #1 (symbol -> symbol_id)

- Change: migrated symbol from `std::string` to `uint32_t symbol_id` in Tick/Order/Trade/Portfolio.
- Why: reduce string hash/compare/copy overhead on hot path.
- Benchmark: `--bench`, Release, `N=1,000,000`, same dataset.
- Result: `ticks_per_sec` improved from ~`1.2e7` to ~`1.8e7` (**~+50%**).
- Next: extract `SymbolTable` and re-benchmark.

## 2026-03-14 - Profiling Run #1
- Added per-tick phase timing in replay loop; hotspot split: market ~43%, strategy ~57%.
- Throughput dropped to ~4.5e6 ticks/sec due to timing overhead (2x `steady_clock::now()` per tick).
- This is a profiling-only run, not comparable with baseline throughput runs.

## 2026-03-14 - Profiling Run #2 (sampled)

- Change: switched phase timing from each-tick to sampled, and added `sampled_events`.
- Result: throughput recovered from ~`4.5e6` to ~`1.2e7` ticks/sec (stable sampled runs).
- Hotspot: `market ~40%`, `strategy ~60%`.
- Next: optimize hot path in replay/matching engine.

## 2026-03-14 - Opt #3 (remove trade vector)
- Removed `vector<Trade>` from `MatchingEngine` hot path, kept trade count + `portfolio.on_trade`.
- Improvement: median `ticks_per_sec` increased from `1.16e7` to `1.57e7` (**+35.3%**).
- Next: optimize replay/matching loop further.