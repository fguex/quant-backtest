#ifndef POSITION_HPP
#define POSITION_HPP

#include <string>

namespace backtest {

/**
 * @brief Represents a trading position in a single security
 * 
 * Tracks quantity, entry price, current price, and calculates P&L.
 * Supports both long and short positions.
 */
class Position {
public:
    Position(const std::string& symbol, int quantity, double entry_price);
    
    // Update current market price
    void update_price(double current_price);
    
    // Add to position (positive = buy, negative = sell)
    void adjust_quantity(int quantity_change, double price);
    
    // Getters
    std::string symbol() const { return symbol_; }
    int quantity() const { return quantity_; }
    double entry_price() const { return entry_price_; }
    double current_price() const { return current_price_; }
    
    // P&L calculations
    double market_value() const;          // Current value
    double unrealized_pnl() const;        // Profit/loss not yet realized
    double cost_basis() const;            // Original investment
    
    // Check if position is closed
    bool is_flat() const { return quantity_ == 0; }

private:
    std::string symbol_;
    int quantity_;           // Positive = long, negative = short
    double entry_price_;     // Average entry price
    double current_price_;   // Latest market price
};

} // namespace backtest

#endif // POSITION_HPP