cmake_minimum_required(VERSION 3.20)

project(market_data_replay_engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(replay_core
    src/market_data_loader.cpp
    src/replay_engine.cpp
    src/parallel_replay_executor.cpp
)

target_include_directories(replay_core PUBLIC include)

add_executable(replay
    src/main.cpp
)

target_link_libraries(replay PRIVATE replay_core)

enable_testing()
add_subdirectory(tests)