#include "portfolio.hpp"
#include <cmath>

namespace backtest {

Portfolio::Portfolio(double initial_capital)
    : initial_capital_(initial_capital)
    , cash_(initial_capital)
    , realized_pnl_(0.0) {

}

void Portfolio::open_position(const std::string& symbol, int quantity, double price, double commission) {
    // TODO: Implement this function
    // Follow the steps in the header file comments
    
    // Step 1: Calculate cost
    double cost = std::abs(quantity * price) + commission;
    
    // Step 2: Deduct from cash
    cash_ -= cost;
    
    
    // Step 3-5: Handle position creation/update
    if (has_position(symbol)) {
        // Step 4: Position exists - adjust quantity
        positions_.at(symbol).adjust_quantity(quantity, price);
    } else {
        // Step 5: New position - create and add to map
        positions_.emplace(symbol, Position(symbol, quantity, price));
    }
}

void Portfolio::close_position(const std::string& symbol, int quantity, double price, double commission) {
    // TODO: Implement this function
    // Follow the steps in the header file comments
    
    // Step 1: Check if position exists
    if (has_position(symbol) == false) {
        return;  // Nothing to close
    }
    
    // Step 2: Get position reference
    Position& pos = positions_.at(symbol);
    double pnl = (price - pos.entry_price()) * quantity;    // Step 3: Calculate realized P&L
    
    // Step 4: Update realized_pnl_
    realized_pnl_ += pnl;
    
    // Step 5: Calculate proceeds
    double proceeds = std::abs(quantity * price) - commission;
    
    // Step 6: Add to cash
    cash_ += proceeds;
    
    
    // Step 7: Adjust position
    pos.adjust_quantity(-quantity, price);
    
    // Step 8: Remove if flat
    if (pos.is_flat()) {
        positions_.erase(symbol);
    }
}

void Portfolio::update_prices(const Bar& bar) {
    // TODO: Implement this function
    // Check if position exists, then update its price
    if (has_position(bar.symbol)){
        positions_.at(bar.symbol).update_price(bar.close);
    }

}

bool Portfolio::has_position(const std::string& symbol) const {
    // TODO: Implement this function
    // Use positions_.find(symbol) != positions_.end()
    return positions_.find(symbol) != positions_.end();
}

const Position* Portfolio::get_position(const std::string& symbol) const {
    auto it = positions_.find(symbol);
    return (it != positions_.end()) ? &it->second : nullptr;
}

double Portfolio::total_value() const {
    // TODO: Implement this function
    // Sum cash plus all position market values
    auto it = positions_.begin();
    double total = cash_;
    for (; it != positions_.end(); ++it){
        total += it->second.market_value();
    }
    return total;
    }

double Portfolio::total_pnl() const {
    // Total P&L = realized + unrealized
    return realized_pnl_ + unrealized_pnl();
}

double Portfolio::unrealized_pnl() const {
    // TODO: Implement this function
    // Sum unrealized P&L from all positions
    double pnl = 0.0;
    for (const auto& [symbol, pos] : positions_) {
        pnl += pos.unrealized_pnl();
    }
    return pnl;
}

} // namespace backtest