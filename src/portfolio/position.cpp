/**
 * @file position.cpp
 * @brief Implementation of Position class for tracking trading positions
 * 
 * Handles position tracking, P&L calculations, and position adjustments
 * for both long and short positions in a single security.
 */

#include "position.hpp"
#include <cmath>

namespace backtest {

// ============================================================================
// CONSTRUCTOR
// ============================================================================

/**
 * @brief Initializes a new position with given parameters
 * @param symbol Ticker symbol
 * @param quantity Initial number of shares
 * @param entry_price Initial entry price
 * 
 * Sets the current price equal to entry price initially.
 */
Position::Position(const std::string& symbol, int quantity, double entry_price)
    : symbol_(symbol)
    , quantity_(quantity)
    , entry_price_(entry_price)
    , current_price_(entry_price) {
    // All member variables initialized in initializer list
}


// ============================================================================
// PRICE UPDATE
// ============================================================================

/**
 * @brief Updates the current market price for this position
 * @param current_price New market price
 * 
 * TODO: Implement this function
 * This is the simplest function - just update the member variable.
 * 
 * Task:
 * - Set current_price_ to the new price
 */
void Position::update_price(double current_price) {
    // TODO: Update current_price_ with the new price
    current_price_ = current_price;
}


// ============================================================================
// POSITION ADJUSTMENT
// ============================================================================

/**
 * @brief Adjusts position quantity and recalculates average entry price
 * @param quantity_change Amount to add (positive) or remove (negative)
 * @param price Price at which adjustment happens
 * 
 * TODO: Implement this function (MOST COMPLEX)
 * This handles adding to, reducing, or closing positions.
 * 
 * Key Concepts:
 * - Adding to position (same direction): Recalculate average entry price
 * - Reducing position (opposite direction): Keep original entry price
 * - Closing position completely: Set entry price to 0
 * 
 * Steps:
 * 1. Determine if adding or reducing position
 *    Hint: Check if quantity_ and quantity_change have same sign
 *    bool adding = (quantity_ > 0 && quantity_change > 0) || 
 *                  (quantity_ < 0 && quantity_change < 0);
 * 
 * 2. If ADDING to position:
 *    - Calculate total cost: (quantity_ * entry_price_) + (quantity_change * price)
 *    - Update quantity: quantity_ += quantity_change
 *    - Calculate new average: entry_price_ = total_cost / quantity_
 * 
 * 3. If REDUCING position:
 *    - Just update quantity: quantity_ += quantity_change
 *    - Keep original entry_price_ (unless quantity becomes 0)
 * 
 * 4. Handle position close:
 *    - If quantity_ becomes 0, set entry_price_ = 0.0
 * 
 * 5. Always update current price:
 *    - current_price_ = price
 * 
 * Examples:
 * - Buy 100 @ $50, then buy 50 @ $60:
 *   New avg = (100*50 + 50*60) / 150 = $53.33
 * 
 * - Buy 100 @ $50, then sell 30 @ $55:
 *   Quantity becomes 70, entry stays $50
 */
void Position::adjust_quantity(int quantity_change, double price) {
    // TODO: Step 1 - Determine if adding or reducing
    bool adding = (quantity_ >0 && quantity_change > 0) ||
                  (quantity_ < 0 && quantity_change < 0);
    
    // TODO: Step 2 - If adding to position, recalculate average entry price
    if (adding) {
        double total_cost = (quantity_ * entry_price_) + (quantity_change * price);
        quantity_ += quantity_change;
        entry_price_ = total_cost / quantity_;
    }
    
    // TODO: Step 3 - If reducing position, just update quantity
    else {
        quantity_ += quantity_change;
    }
    
    // TODO: Step 4 - Handle complete close (quantity becomes 0)
    if (quantity_ == 0) {
        entry_price_ = 0.0;
    }
    
    // TODO: Step 5 - Always update current price
    current_price_ = price;
}


// ============================================================================
// P&L CALCULATIONS
// ============================================================================

/**
 * @brief Calculates the current market value of the position
 * @return Market value = quantity * current_price
 * 
 * TODO: Implement this function
 * 
 * Task:
 * - Multiply quantity_ by current_price_
 * - Return the result
 * 
 * Examples:
 * - Long 100 shares @ current $55: 100 * 55 = $5,500
 * - Short 100 shares @ current $55: -100 * 55 = -$5,500
 */
double Position::market_value() const {
    // TODO: Calculate and return market value
    return quantity_ * current_price_;
}


/**
 * @brief Calculates unrealized profit or loss
 * @return Unrealized P&L = (current_price - entry_price) * quantity
 * 
 * TODO: Implement this function
 * 
 * Tasks:
 * 1. If position is flat (quantity_ == 0), return 0.0
 * 2. Calculate P&L: (current_price_ - entry_price_) * quantity_
 * 3. Return the result
 * 
 * Examples:
 * - Long 100 @ $50, now $55: (55-50) * 100 = +$500 profit
 * - Long 100 @ $50, now $45: (45-50) * 100 = -$500 loss
 * - Short 100 @ $50, now $55: (55-50) * -100 = -$500 loss
 * - Short 100 @ $50, now $45: (45-50) * -100 = +$500 profit
 * 
 * Note: The formula works for both long and short positions!
 */
double Position::unrealized_pnl() const {
    // TODO: Step 1 - Check if position is flat
    if ( is_flat() ) {
        return 0.0;
    }
    
    // TODO: Step 2 - Calculate unrealized P&L

   
    return (current_price_ - entry_price_) * quantity_;
}


/**
 * @brief Calculates the absolute cost basis of the position
 * @return Absolute value of (quantity * entry_price)
 * 
 * TODO: Implement this function
 * 
 * Task:
 * - Calculate quantity_ * entry_price_
 * - Return the absolute value using std::abs()
 * 
 * Examples:
 * - Long 100 @ $50: abs(100 * 50) = $5,000
 * - Short 100 @ $50: abs(-100 * 50) = $5,000
 * 
 * Why absolute? Cost basis is always positive regardless of direction.
 */
double Position::cost_basis() const {
    // TODO: Calculate absolute cost basis
    return std::abs(quantity_ * entry_price_);
}

} // namespace backtest
