#include "data/data_handler.hpp"
#include "strategy/sma_strategy.hpp"
#include <iostream>

int main() {
    backtest::DataHandler data;
    backtest::SMAStrategy strategy(3, 5);  // 3-day short, 5-day long MA
    
    if (!data.load_csv("../data/sample_data.csv")) {
        std::cout << "Failed to load data" << std::endl;
        return 1;
    }
    
    std::cout << "Running strategy: " << strategy.get_name() << std::endl;
    std::cout << "Loaded " << data.size() << " bars" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    
    while (data.has_next()) {
        backtest::Bar bar = data.get_next_bar();
        strategy.on_new_bar(bar);
        backtest::Signal signal = strategy.generate_signal();
        
        std::string signal_str;
        if (signal.type == backtest::SignalType::BUY) signal_str = "BUY ";
        else if (signal.type == backtest::SignalType::SELL) signal_str = "SELL";
        else signal_str = "HOLD";
        
        std::cout << bar.timestamp << " | Close: " << bar.close 
                  << " | Signal: " << signal_str << std::endl;
    }
    
    return 0;
}
    
