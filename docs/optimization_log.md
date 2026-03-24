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


## 2026-03-16 - Opt #11 (first sharded parallel prototype)

- Change: added first sharded parallel bench prototype by partitioning ticks by `symbol_id % shard_count` and running each shard on a separate thread.
- Result (`opt_v4_0_parallel_sharded_replay`, 5 runs): median `ticks_per_sec = 3.941e8`.
- Improvement: vs `opt_v3_7_inline_matching_engine` baseline (`7.356e8`), throughput decreased by **~46.4%**.
- Analysis: this prototype creates and joins worker threads inside each benchmark iteration, so thread management overhead dominates and outweighs parallel execution benefit.
- Next: move thread creation outside the `N` loop and let each worker process its shard repeatedly.


## 2026-03-18 - Opt #12 (parallel persistent workers)

- Change: moved thread creation outside the benchmark loop; each worker thread is created once and repeatedly processes its own shard for `N` iterations.
- Result (`opt_v4_1_parallel_persistent_workers`, 5 runs): median `ticks_per_sec = 1.017e9`.
- Improvement: vs `opt_v4_0_parallel_sharded_replay` baseline (`3.941e8`), throughput improved by **~158%**.
- Improvement: vs `opt_v3_7_inline_matching_engine` single-thread baseline (`7.356e8`), throughput improved by **~38%**.(Only 38% faster than single-threaded — needs further investigation.)
- Analysis: the first parallel prototype was dominated by per-iteration thread creation/join overhead; persistent workers removed that overhead and made parallel replay beneficial.

## 2026-03-18 - Parallel Scaling Note

- Observation: parallel persistent workers improved throughput by only ~38% vs the single-thread baseline.
- Cause: shard distribution on the current benchmark input was highly imbalanced (`75000 / 25000 / 0 / 0`), so only part of the workers received useful work.
- Next: use a more balanced multi-symbol dataset for parallel scaling evaluation.

## 2026-03-19 - Parallel Scaling on Balanced Input

- Setup: switched parallel benchmark input to a balanced 4-symbol dataset and measured scaling with `1 / 2 / 4` shards.
- Result: `1-shard` baseline was stable around `7.7e8` ticks/sec; `4-shard` runs mostly reached `3.2e9 ~ 3.3e9` ticks/sec, close to linear scaling on balanced input.
- Observation: `2-shard` runs showed large variance on Windows and were less stable than `1-shard` and `4-shard`.
- Conclusion: the parallel replay model itself scales well under balanced workload; earlier low speedup was mainly caused by shard imbalance in the original input.
- Next: re-run `1 / 2 / 4` scaling on Linux and check scheduling-related metrics if the `2-shard` variance remains.

## 2026-03-19 - Parallel Scaling Validation

- Windows balanced-input runs showed that the parallel model scaled, but 2-shard and 4-shard results still had noticeable variance.
- A Windows affinity experiment reduced some variance (especially for 2-shard), suggesting scheduler / CPU migration noise affected benchmark stability.
- On a 4-vCPU Linux VM, balanced-input scaling became much clearer: `1-shard ~4.6e8`, `2-shard ~9.6e8`, `4-shard ~1.9e9` ticks/sec, close to linear scaling.
- Conclusion: the sharded persistent-worker design scales well under balanced workload; earlier instability was largely caused by workload imbalance and platform scheduling noise.

## 2026-03-20 - Opt #13 (preallocate portfolio capacity)

- Preallocated `Portfolio` storage before replay and removed per-tick capacity check from `update_to_market()`.
- `perf` motivation: `Portfolio::update_to_market()` / `ensure_symbol_capacity()` were major hot spots.
- Linux VM benchmark (`opt_v4_6_linux_vm_remove_update_capacity_check_4shard`, 5 runs): median `ticks_per_sec` improved from `1.889e9` to `2.480e9` (**~+31.3%**).
- Windows dev-machine note: median `ticks_per_sec` also improved from `3.278e9` to `3.516e9` (**~+7.2%**).
- Going forward, Linux VM is the formal benchmark baseline; Windows runs are for quick development validation only.
- Next: re-profile the new Linux baseline.


## 2026-03-21 - Opt #14 (remove order_fill capacity check)

- Removed per-order `ensure_symbol_capacity()` from `Portfolio::on_order_fill()` after portfolio capacity was already preallocated before replay.
- Linux VM benchmark (`opt_v4_7_linux_vm_remove_order_fill_capacity_check_4shard`, 5 runs): median `ticks_per_sec` improved from `2.489e9` to `2.646e9` (**~+6.3%**).
- Next: re-profile the new Linux baseline.

## 2026-03-21 - Refactor (extract parallel bench executor)

- Extracted parallel benchmark execution from `main.cpp` into `parallel_replay_executor`.
- This is a structural refactor, not a new optimization baseline.
- Current Linux VM runs stayed in a similar range with a small regression (~`-6%`).
- Next: continue profiling from the current Linux VM benchmark path.


## 2026-03-24 - Parallel scaling check (`perf stat`, Linux VM)

- Measured `1 / 2 / 4` shard runs with `perf stat -e task-clock,context-switches,cpu-migrations,page-faults`.
- `CPUs utilized` scaled from ~`1.0` to ~`2.0` to ~`3.8`, consistent with healthy parallel utilization on the current 4-vCPU Linux VM.
- `ticks_per_sec` scaled from ~`7e8` to ~`1.5e9` to ~`2.9e9`, close to linear at this stage.
- No significant context-switch or CPU-migration noise was observed in these runs.
- Page-fault counts were low and stable across runs, suggesting paging was not a dominant factor in the current VM setup.