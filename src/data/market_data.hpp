/**
 * @file market_data.hpp
 * @brief Core market data structures for backtesting
 * 
 * Defines the fundamental data structure (Bar) representing a single 
 * period of market activity with OHLCV (Open, High, Low, Close, Volume) data.
 */

#ifndef MARKET_DATA_HPP
#define MARKET_DATA_HPP
#include <string>

namespace backtest {

/**
 * @struct Bar
 * @brief Represents a single candlestick/bar of market data
 * 
 * Contains OHLCV (Open, High, Low, Close, Volume) price data for a specific time period.
 * This is the fundamental unit of price information used throughout the backtesting system.
 */
struct Bar {
    std::string timestamp;  ///< ISO 8601 format timestamp (e.g., "2024-01-15T09:30:00")
    double open;            ///< Opening price for the period
    double high;            ///< Highest price during the period
    double low;             ///< Lowest price during the period
    double close;           ///< Closing price for the period
    double volume;          ///< Trading volume during the period

    /**
     * @brief Constructs a Bar with all OHLCV data
     * @param ts Timestamp string
     * @param o Opening price
     * @param h High price
     * @param l Low price
     * @param c Closing price
     * @param v Trading volume
     */
    Bar(const std::string& ts, double o, double h, double l, double c, double v)
        : timestamp(ts), open(o), high(h), low(l), close(c), volume(v) {}

    /**
     * @brief Default constructor - initializes with zero/empty values
     */
    Bar() : timestamp(""), open(0), high(0), low(0), close(0), volume(0) {}

};

} // namespace backtest
#endif // MARKET_DATA_HPP

