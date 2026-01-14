cmake_minimum_required(VERSION 3.20)

project(market_data_replay_engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(replay
    src/main.cpp
    src/market_data_loader.cpp
)

target_include_directories(replay PRIVATE include)