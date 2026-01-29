#ifndef STRATEGY_BASE_HPP
#define STRATEGY_BASE_HPP
#include "../data/market_data.hpp"
#include <string>

namespace backtest {
    enum class SignalType {
        BUY,
        SELL,
        HOLD
    };

    struct Signal {
        SignalType type;
        std::string timestamp;
        double strength;

        Signal(SignalType t, const std::string& ts, double str)
            : type(t), timestamp(ts), strength(str) {}
    };
class Strategy{
    protected: 
    std::string name_;

    public:
    Strategy(const std::string& name): name_(name){}
    virtual ~Strategy() = default;

    // Pure virtual - must be implemented by derived classes
    virtual void on_new_bar(const Bar& bar) = 0;
    virtual Signal generate_signal() const = 0;
    
    // Getters
    std::string get_name() const { return name_; }

}
}