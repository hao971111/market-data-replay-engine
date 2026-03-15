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

## 2026-03-14 - Opt #4 (replay split stats path): no clear throughput gain vs v3 (median ~15.44M vs ~15.68M).

## 2026-03-14 - Benchmark Method Update (fixed total events)
- Changed benchmark method to fixed total events: `TARGET_TOTAL_EVENTS = 1e9` with expanded input (`target_ticks = 100000`).
- Comparison baseline is now runs with `N=10000` and total `ticks=1,000,000,000` (ignore older runs with different total work).
- Current stable baseline (`opt_v3_with_100k_bench_input`): ~`1.36e8` to `1.39e8` ticks/sec.

## Current Throughput Baseline
- Baseline version: `opt_v3_throughput_clean`
- Benchmark setup: fixed workload (`TARGET_TOTAL_EVENTS=1e9`, `target_ticks=100000`, `N=10000`)
- Baseline performance (3 runs): median `ticks_per_sec = 1.642e8`

## 2026-03-14 - Opt #5 (on_order_fill no temporary trade object)

- Change: in `MatchingEngine::on_order`, removed temporary `Trade` construction and call `portfolio.on_order_fill(order)` directly.
- Benchmark setup: fixed workload (`TARGET_TOTAL_EVENTS=1e9`, `target_ticks=100000`, `N=10000`).
- Result (`opt_v3_1_on_order_fill_no_trade_obj`, 5 runs): `ticks_per_sec` range `1.696e8 ~ 1.745e8`, median `1.713e8`.
- Improvement: vs previous clean baseline (`opt_v3_throughput_clean` median `1.642e8`), throughput improved by **~4.3%**.

## 2026-03-15 - Opt #6 (devirtualized hot path)

- Change: added `run_fast<StrategyT, SinkT>()`; bench uses concrete `FastCountingStrategy` + `MatchingEngine`.
- Result (`opt_v3_3_devirtualized_hot_path`, 5 runs): median `ticks_per_sec = 1.937e8`.
- Improvement: vs `opt_v3_1` baseline (`1.713e8`), **~+13.1%**.

## 2026-03-15 - Opt #7 (SoA tick store)

- Change: added `TickStore` (SoA) and `run_fast_soa()` for bench path.
- Result (`opt_v3_4_soa_tick_store`, 5 runs): median `ticks_per_sec = 1.922e8`.
- Improvement: vs `opt_v3_3_devirtualized_hot_path` baseline (`1.942e8`), no gain (**~ -1.0%**).
- Next: remove temporary `Tick` reconstruction in hot path.

## 2026-03-15 - Opt #8 (SoA direct fields)

- Change: extended SoA bench path by removing temporary `Tick` reconstruction and calling strategy with direct fields.
- Result (`opt_v3_5_soa_direct_fields`, 5 runs): median `ticks_per_sec = 1.899e8`.
- Improvement: vs `opt_v3_3_devirtualized_hot_path` baseline (`1.942e8`), throughput decreased by **~2.2%**.
- Conclusion: under current workload, SoA path did not outperform AoS hot path.

## 2026-03-15 - Profiling Run #3 (Linux perf report)

- Ran `perf report` on Linux benchmark build.
- Hotspots: `main` ~53% (likely inlined hot loop), `std::__detail::_Map_base...` ~30%, `MatchingEngine::on_order` ~17%.
- Finding: `unordered_map` access in portfolio path is a major hotspot.
- Next: replace portfolio `unordered_map` with direct-indexed `vector`.

## 2026-03-15 - Opt #9 (portfolio unordered_map -> vector)

- Change: replaced `Portfolio` state storage from `unordered_map<uint32_t, ...>` to direct-indexed `std::vector` by `symbol_id`.
- Why: Linux `perf report` showed hash map access as a major hotspot in portfolio path.
- Benchmark setup: fixed workload (`TARGET_TOTAL_EVENTS=1e9`, `target_ticks=100000`, `N=10000`).
- Result (`opt_v3_6_portfolio_vector`, 5 runs): median `ticks_per_sec = 3.214e8`.
- Improvement: vs `opt_v3_3_devirtualized_hot_path` baseline (`1.942e8`), throughput improved by **~65.5%**.

## 2026-03-15 - Opt #10 (inline matching engine hot path)

- Change: moved `MatchingEngine::on_order` implementation into header to make the hot path easier to inline.
- Why: after portfolio vector optimization, Linux `perf report` showed `MatchingEngine::on_order` as the new top hotspot.
- Benchmark setup: fixed workload (`TARGET_TOTAL_EVENTS=1e9`, `target_ticks=100000`, `N=10000`).
- Result (`opt_v3_7_inline_matching_engine`, 5 runs): median `ticks_per_sec = 7.356e8`.
- Improvement: vs `opt_v3_6_portfolio_vector` baseline (`3.214e8`), throughput improved by **~128.9%**.
- Note: next step is to verify whether the previous call was eliminated via assembly comparison on Linux.