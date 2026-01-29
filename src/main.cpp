#include "data/data_handler.hpp"
#include <iostream>

int main() {
    backtest::DataHandler data;
    
    if (data.load_csv("../data/sample_data.csv")) {
        std::cout << "Successfully loaded " << data.size() << " bars" << std::endl;
        
        int count = 0;
        while (data.has_next() && count < 5) {
            backtest::Bar bar = data.get_next_bar();
            std::cout << bar.timestamp << " | "
                      << "O: " << bar.open << " "
                      << "H: " << bar.high << " "
                      << "L: " << bar.low << " "
                      << "C: " << bar.close << " "
                      << "V: " << bar.volume << std::endl;
            count++;
        }
    } else {
        std::cout << "Failed to load data" << std::endl;
        return 1;
    }
    
    return 0;
}
