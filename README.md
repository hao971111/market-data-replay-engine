## Market Data Replay Engine (C++)

### Build (Windows + MinGW g++)

```powershell
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

### (Optional) If CMake can't find g++

Make sure your MinGW `bin` directory is in `PATH`, or specify the compiler explicitly:

```powershell
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++
cmake --build build -j
```

### Run

```powershell
.\build\replay.exe
```