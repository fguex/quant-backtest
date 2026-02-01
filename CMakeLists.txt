cmake_minimum_required(VERSION 3.10)
project(QuantBacktest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(src src/data src/strategy)

add_executable(backtest
    src/main.cpp
    src/data/data_handler.cpp
    src/strategy/sma_strategy.cpp
)
