#ifndef PORTFOLIO_HPP
#define PORTFOLIO_HPP

#include "position.hpp"
#include "../data/market_data.hpp"
#include <map>
#include <string>

namespace backtest {

/**
 * @brief Manages portfolio of positions and cash balance
 * 
 * The Portfolio tracks:
 * - Cash available for trading
 * - All open positions
 * - Realized P&L (from closed trades)
 * - Unrealized P&L (from open positions)
 * 
 * Total Portfolio Value = Cash + Sum of all position market values
 */
class Portfolio {
public:
    /**
     * @brief Construct portfolio with initial capital
     * @param initial_capital Starting cash amount
     */
    explicit Portfolio(double initial_capital);
    
    /**
     * @brief Open or add to a position
     * @param symbol Ticker symbol
     * @param quantity Number of shares (positive = buy, negative = short)
     * @param price Execution price
     * @param commission Transaction cost (default 0)
     * 
     * TODO: Implement this function
     * Steps:
     * 1. Calculate total cost: abs(quantity * price) + commission
     * 2. Deduct from cash: cash_ -= cost
     * 3. Check if position already exists (use has_position())
     * 4. If exists: call positions_.at(symbol).adjust_quantity(quantity, price)
     * 5. If new: create new Position and add to positions_ map
     *    Use: positions_.emplace(symbol, Position(symbol, quantity, price))
     */
    void open_position(const std::string& symbol, int quantity, double price, double commission = 0.0);
    
    /**
     * @brief Close or reduce a position
     * @param symbol Ticker symbol
     * @param quantity Number of shares to close
     * @param price Execution price
     * @param commission Transaction cost (default 0)
     * 
     * TODO: Implement this function
     * Steps:
     * 1. Check if position exists (if not, return early)
     * 2. Get reference to position: Position& pos = positions_.at(symbol)
     * 3. Calculate realized P&L: (price - pos.entry_price()) * quantity
     * 4. Add to realized_pnl_: realized_pnl_ += pnl
     * 5. Calculate proceeds: abs(quantity * price) - commission
     * 6. Add to cash: cash_ += proceeds
     * 7. Adjust position: pos.adjust_quantity(-quantity, price)
     * 8. If position is now flat (pos.is_flat()), remove from map:
     *    positions_.erase(symbol)
     */
    void close_position(const std::string& symbol, int quantity, double price, double commission = 0.0);
    
    /**
     * @brief Update position prices with new market data
     * @param bar Market data bar
     * 
     * TODO: Implement this function
     * Steps:
     * 1. Check if we have a position in this symbol (use has_position())
     * 2. If yes, update its price: positions_.at(bar.symbol).update_price(bar.close)
     */
    void update_prices(const Bar& bar);
    
    /**
     * @brief Check if portfolio has a position in symbol
     * @param symbol Ticker to check
     * @return true if position exists
     * 
     * TODO: Implement this function
     * Hint: Use positions_.find(symbol) != positions_.end()
     */
    bool has_position(const std::string& symbol) const;
    
    /**
     * @brief Get pointer to position (or nullptr if doesn't exist)
     * @param symbol Ticker symbol
     * @return Pointer to Position or nullptr
     */
    const Position* get_position(const std::string& symbol) const;
    
    // Simple getters (already implemented)
    double cash() const { return cash_; }
    double realized_pnl() const { return realized_pnl_; }
    const std::map<std::string, Position>& positions() const { return positions_; }
    
    /**
     * @brief Calculate total portfolio value
     * @return Cash + sum of all position market values
     * 
     * TODO: Implement this function
     * Steps:
     * 1. Start with value = cash_
     * 2. Loop through all positions (use range-based for loop)
     *    for (const auto& [symbol, pos] : positions_) { ... }
     * 3. Add each position's market value: value += pos.market_value()
     * 4. Return total value
     */
    double total_value() const;
    
    /**
     * @brief Calculate total P&L (realized + unrealized)
     * @return Total profit/loss
     * 
     * TODO: Implement this function
     * Formula: realized_pnl_ + unrealized_pnl()
     */
    double total_pnl() const;
    
    /**
     * @brief Calculate unrealized P&L across all positions
     * @return Sum of unrealized P&L from all open positions
     * 
     * TODO: Implement this function
     * Steps:
     * 1. Initialize pnl = 0.0
     * 2. Loop through positions
     * 3. Add each position's unrealized P&L: pnl += pos.unrealized_pnl()
     * 4. Return total
     */
    double unrealized_pnl() const;

private:
    double initial_capital_;    // Starting cash
    double cash_;               // Current cash available
    double realized_pnl_;       // Locked-in profits/losses
    std::map<std::string, Position> positions_;  // All open positions
};

} // namespace backtest

#endif // PORTFOLIO_HPP