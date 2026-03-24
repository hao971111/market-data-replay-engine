# High-Performance Quant Backtesting & Market Data Replay Engine

A C++ project for replaying historical market data and running backtests on a single machine.

This project focuses on the system core of a backtesting engine rather than strategy complexity: market data loading, timestamp-ordered replay, strategy callbacks, order matching, portfolio updates, benchmarking, and performance optimization.

Pipeline:

`Market Data Loader -> Replay Engine -> Strategy -> Matching Engine -> Portfolio`

## Build

### Windows

```powershell
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

If CMake cannot find `g++`, specify the compiler explicitly:

```powershell
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++
cmake --build build -j
```

### Linux

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

## Run

### Functional Backtest

Windows:

```powershell
.\build\replay.exe --bt
```

Linux:

```bash
./build/replay --bt
```

### Benchmark

Windows:

```powershell
.\build\replay.exe --bench
```

Linux:

```bash
./build/replay --bench
```
## Testing

### Windows

```powershell
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
ctest --test-dir build --output-on-failure
```

### Linux

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
ctest --test-dir build --output-on-failure
```

## Notes

- Benchmark history: `bench_results.csv`
- Optimization notes: `docs/optimization_log.md`
- Linux profiling: `perf stat`, `perf record`, `perf report`
