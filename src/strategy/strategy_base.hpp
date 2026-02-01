/**
 * @file strategy_base.hpp
 * @brief Base classes and types for trading strategy implementation
 * 
 * Defines the abstract Strategy interface that all trading strategies must implement,
 * along with signal types and structures for communicating trading decisions.
 */

#ifndef STRATEGY_BASE_HPP
#define STRATEGY_BASE_HPP
#include "../data/market_data.hpp"
#include <string>

namespace backtest {
    
    /**
     * @enum SignalType
     * @brief Types of trading signals a strategy can generate
     */
    enum class SignalType {
        BUY,   ///< Buy signal - enter or increase long position
        SELL,  ///< Sell signal - enter short or exit long position
        HOLD   ///< Hold signal - maintain current position, no action
    };

    /**
     * @struct Signal
     * @brief Represents a trading signal with type, timing, and confidence
     * 
     * Encapsulates all information about a trading decision made by a strategy.
     */
    struct Signal {
        SignalType type;        ///< Type of signal (BUY, SELL, or HOLD)
        std::string timestamp;  ///< When the signal was generated
        double strength;        ///< Signal confidence/strength (0.0 to 1.0)

        /**
         * @brief Constructs a trading signal
         * @param t Signal type
         * @param ts Timestamp of signal generation
         * @param str Signal strength/confidence (0.0 = no confidence, 1.0 = full confidence)
         */
        Signal(SignalType t, const std::string& ts, double str)
            : type(t), timestamp(ts), strength(str) {}
    };

    /**
     * @class Strategy
     * @brief Abstract base class for all trading strategies
     * 
     * Defines the interface that all trading strategies must implement.
     * Derived classes should override on_new_bar() to process price data
     * and generate_signal() to return trading decisions.
     */
    class Strategy {
    protected: 
        std::string name_;  ///< Human-readable strategy name

    public:
        /**
         * @brief Constructs a strategy with a given name
         * @param name Descriptive name for the strategy
         */
        Strategy(const std::string& name) : name_(name) {}
        
        /**
         * @brief Virtual destructor for proper cleanup of derived classes
         */
        virtual ~Strategy() = default;

        /**
         * @brief Process a new bar of market data (pure virtual)
         * @param bar The new market data to process
         * 
         * This method is called for each new bar in the backtest.
         * Derived classes should update internal state and recalculate indicators here.
         */
        virtual void on_new_bar(const Bar& bar) = 0;
        
        /**
         * @brief Generate the current trading signal (pure virtual)
         * @return Signal object with trading decision
         * 
         * Returns the most recent trading signal based on current market state.
         * Should be called after on_new_bar() to get the latest decision.
         */
        virtual Signal generate_signal() const = 0;
        
        /**
         * @brief Returns the strategy name
         * @return Strategy name string
         */
        std::string get_name() const { return name_; }
    };

} // namespace backtest

#endif // STRATEGY_BASE_HPP
