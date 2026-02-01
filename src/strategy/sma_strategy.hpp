/**
 * @file sma_strategy.hpp
 * @brief Simple Moving Average (SMA) crossover trading strategy
 * 
 * Implements a classic technical analysis strategy using two moving averages
 * of different periods to identify trend changes and generate trading signals.
 */

#ifndef SMA_STRATEGY_HPP
#define SMA_STRATEGY_HPP

#include "strategy_base.hpp"
#include <deque>

namespace backtest {

/**
 * @class SMAStrategy
 * @brief Trading strategy based on Simple Moving Average crossovers
 * 
 * Uses two SMAs with different window sizes:
 * - Short-term SMA: Responds quickly to price changes
 * - Long-term SMA: Provides overall trend direction
 * 
 * Signal Generation:
 * - BUY: When short SMA crosses above long SMA (golden cross)
 * - SELL: When short SMA crosses below long SMA (death cross)
 * - HOLD: When SMAs are equal or insufficient data available
 */
class SMAStrategy : public Strategy {
private:
    size_t short_window_;                ///< Short-term MA window size (e.g., 10 days)
    size_t long_window_;                 ///< Long-term MA window size (e.g., 50 days)
    std::deque<double> short_prices_;    ///< Sliding window of prices for short MA
    std::deque<double> long_prices_;     ///< Sliding window of prices for long MA
    Signal current_signal_;              ///< Most recent trading signal
    
    /**
     * @brief Calculates the Simple Moving Average for a price window
     * @param prices Deque containing historical prices
     * @return Average of all prices in the deque
     */
    double calculate_sma(const std::deque<double>& prices);
    
public:
    /**
     * @brief Constructs an SMA strategy with specified window sizes
     * @param short_win Size of short-term moving average window (default: 10)
     * @param long_win Size of long-term moving average window (default: 50)
     */
    SMAStrategy(size_t short_win = 10, size_t long_win = 50);
    
    /**
     * @brief Processes new market data and updates signals
     * @param bar New OHLCV bar to process
     * 
     * Updates price windows, calculates SMAs, and generates trading signals
     * based on the relationship between short and long moving averages.
     */
    void on_new_bar(const Bar& bar) override;
    
    /**
     * @brief Returns the current trading signal
     * @return Most recent Signal object
     */
    Signal generate_signal() const override;
};

} // namespace backtest

#endif // SMA_STRATEGY_HPP  