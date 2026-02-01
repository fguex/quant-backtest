/**
 * @file sma_strategy.cpp
 * @brief Implementation of Simple Moving Average (SMA) crossover strategy
 * 
 * This strategy generates trading signals based on the relationship between
 * short-term and long-term moving averages:
 * - BUY signal: Short MA crosses above Long MA (uptrend detected)
 * - SELL signal: Short MA crosses below Long MA (downtrend detected)
 * - HOLD signal: MAs are equal or insufficient data
 */

#include "sma_strategy.hpp"
#include <numeric>  // For std::accumulate

namespace backtest {

/**
 * @brief Constructs an SMA strategy with specified window sizes
 * @param short_win Size of the short-term moving average window (default: 10)
 * @param long_win Size of the long-term moving average window (default: 50)
 * 
 * Initializes the strategy with a descriptive name and sets the initial signal to HOLD.
 */
SMAStrategy::SMAStrategy(size_t short_win, size_t long_win)
    : Strategy("SMA_" + std::to_string(short_win) + "_" + std::to_string(long_win)),
      short_window_(short_win),
      long_window_(long_win),
      current_signal_(SignalType::HOLD, "", 1.0) {
}


/**
 * @brief Calculates the Simple Moving Average for a given price window
 * @param prices Deque of historical prices
 * @return Average of all prices, or 0.0 if empty
 * 
 * Uses std::accumulate to sum all prices and divides by count.
 * Returns 0.0 for empty input to prevent division by zero.
 */
double SMAStrategy::calculate_sma(const std::deque<double>& prices) {
    if (prices.empty()) {
        return 0.0;  // Prevent division by zero
    }
    
    double sum = std::accumulate(prices.begin(), prices.end(), 0.0);
    return sum / prices.size();
}


/**
 * @brief Processes a new price bar and updates trading signals
 * @param bar Market data containing OHLC prices and timestamp
 * 
 * Algorithm:
 * 1. Add new closing price to both price windows
 * 2. Maintain sliding windows by removing oldest prices when full
 * 3. Wait until enough data is collected (long_window_ bars)
 * 4. Calculate both short and long SMAs
 * 5. Generate signal based on SMA crossover:
 *    - Short > Long: BUY (uptrend)
 *    - Short < Long: SELL (downtrend)
 *    - Equal: HOLD (no clear trend)
 */
void SMAStrategy::on_new_bar(const Bar& bar) {
    
    // Add new price to both windows
    short_prices_.push_back(bar.close);
    long_prices_.push_back(bar.close);
    
    // Maintain sliding window: remove oldest price when window is full
    if (short_prices_.size() > short_window_) {
        short_prices_.pop_front();
    }
    
    if (long_prices_.size() > long_window_) {
        long_prices_.pop_front();
    }
    
    // Wait until we have enough data for the longer window
    // (If long window is full, short window must also be full)
    if (long_prices_.size() < long_window_) {
        current_signal_ = Signal(SignalType::HOLD, bar.timestamp, 0.0);
        return;
    }
    
    // Calculate both moving averages
    double short_sma = calculate_sma(short_prices_);
    double long_sma = calculate_sma(long_prices_);
    
    // Generate signal based on SMA crossover
    if (short_sma > long_sma) {
        // Golden cross: short MA above long MA indicates uptrend
        current_signal_ = Signal(SignalType::BUY, bar.timestamp, 1.0);
    }
    else if (short_sma < long_sma) {
        // Death cross: short MA below long MA indicates downtrend
        current_signal_ = Signal(SignalType::SELL, bar.timestamp, 1.0);
    }
    else {
        // MAs are equal: no clear trend
        current_signal_ = Signal(SignalType::HOLD, bar.timestamp, 0.5);
    }
}


/**
 * @brief Returns the most recent trading signal
 * @return Current Signal object containing type, timestamp, and strength
 */
Signal SMAStrategy::generate_signal() const {
    return current_signal_;
}

} // namespace backtest