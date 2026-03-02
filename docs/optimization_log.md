## 2026-03-02 - Opt #1 (symbol -> symbol_id)

- Change: migrated symbol from `std::string` to `uint32_t symbol_id` in Tick/Order/Trade/Portfolio.
- Why: reduce string hash/compare/copy overhead on hot path.
- Benchmark: `--bench`, Release, `N=1,000,000`, same dataset.
- Result: `ticks_per_sec` improved from ~`1.2e7` to ~`1.8e7` (**~+50%**).
- Next: extract `SymbolTable` and re-benchmark.