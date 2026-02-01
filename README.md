# Quantitative Backtesting Framework

A high-performance C++ backtesting framework for quantitative trading strategies. This system provides a flexible architecture for developing, testing, and analyzing algorithmic trading strategies using historical market data.

## Features

- **Modular Architecture**: Clean separation between data handling, strategy logic, and execution
- **Easy Strategy Development**: Abstract base class for implementing custom trading strategies
- **CSV Data Support**: Load and process historical OHLCV (Open, High, Low, Close, Volume) data
- **Built-in Strategies**: Simple Moving Average (SMA) crossover strategy included
- **Type-Safe Design**: Modern C++17 with strong typing and clear interfaces
- **Well-Documented**: Comprehensive inline documentation and code comments

## Project Structure

```
quant-backtest/
├── src/
│   ├── main.cpp                      # Entry point and backtest execution
│   ├── data/
│   │   ├── market_data.hpp           # OHLCV bar data structures
│   │   ├── data_handler.hpp          # Data loading interface
│   │   └── data_handler.cpp          # CSV file parsing implementation
│   └── strategy/
│       ├── strategy_base.hpp         # Abstract strategy interface
│       ├── sma_strategy.hpp          # SMA crossover strategy header
│       └── sma_strategy.cpp          # SMA crossover implementation
├── data/
│   └── sample_data.csv               # Sample historical market data
├── build/                            # Build artifacts (ignored)
├── CMakeLists.txt                    # CMake build configuration
└── README.md                         # This file
```

## Requirements

- **C++ Compiler**: C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- **CMake**: Version 3.10 or higher
- **Operating System**: Linux, macOS, or Windows

## Building the Project

### Clone the Repository

```bash
git clone https://github.com/fguex/quant-backtest.git
cd quant-backtest
```

### Build with CMake

```bash
mkdir -p build
cd build
cmake ..
make
```

### Run the Backtest

```bash
./backtest
```

## Usage

### Using the Sample Strategy

The included SMA (Simple Moving Average) crossover strategy generates trading signals based on two moving averages:

```cpp
#include "data/data_handler.hpp"
#include "strategy/sma_strategy.hpp"

// Initialize data handler
backtest::DataHandler data;
data.load_csv("../data/sample_data.csv");

// Create strategy with 10-day and 50-day moving averages
backtest::SMAStrategy strategy(10, 50);

// Process each bar
while (data.has_next()) {
    auto bar = data.get_next_bar();
    strategy.on_new_bar(bar);
    auto signal = strategy.generate_signal();
    
    // Process signal...
}
```

### Creating a Custom Strategy

Implement the `Strategy` base class to create your own trading strategy:

```cpp
#include "strategy/strategy_base.hpp"

namespace backtest {

class MyStrategy : public Strategy {
public:
    MyStrategy() : Strategy("MyCustomStrategy") {}
    
    void on_new_bar(const Bar& bar) override {
        // Process new market data
        // Update indicators
        // Generate signals
    }
    
    Signal generate_signal() const override {
        // Return current trading signal
        return current_signal_;
    }
    
private:
    Signal current_signal_;
    // Your strategy state variables
};

} // namespace backtest
```

## Data Format

The system expects CSV files with the following format:

```csv
timestamp,open,high,low,close,volume
2024-01-01,100.0,105.0,99.0,103.0,1000000
2024-01-02,103.0,107.0,102.0,106.0,1200000
...
```

- **timestamp**: Date/time in any string format (e.g., ISO 8601)
- **open**: Opening price for the period
- **high**: Highest price during the period
- **low**: Lowest price during the period
- **close**: Closing price for the period
- **volume**: Trading volume during the period

## Strategy Components

### Signal Types

- **BUY**: Enter long position or increase position size
- **SELL**: Exit long position or enter short position
- **HOLD**: Maintain current position, no action

### Signal Strength

Signals include a strength value (0.0 to 1.0):
- `0.0`: No confidence (e.g., insufficient data)
- `0.5`: Weak signal (e.g., neutral conditions)
- `1.0`: Strong signal (e.g., clear trend)

## SMA Strategy Details

The Simple Moving Average crossover strategy:

- **Golden Cross** (BUY): Short-term MA crosses above long-term MA → Uptrend detected
- **Death Cross** (SELL): Short-term MA crosses below long-term MA → Downtrend detected
- **Neutral** (HOLD): MAs are equal or insufficient data available

Default parameters: 10-day short MA, 50-day long MA (configurable)

## Development Roadmap

- [ ] Additional built-in strategies (RSI, MACD, Bollinger Bands)
- [ ] Performance metrics (Sharpe ratio, max drawdown, win rate)
- [ ] Portfolio management and position sizing
- [ ] Multi-asset backtesting
- [ ] Real-time data feed integration
- [ ] Visualization and reporting tools
- [ ] Order execution simulation with slippage and commissions

## Contributing

Contributions are welcome! Please feel free to submit pull requests or open issues for bugs and feature requests.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Author

**Felix Guex**
- GitHub: [@fguex](https://github.com/fguex)
- Repository: [quant-backtest](https://github.com/fguex/quant-backtest)

## Acknowledgments

- Inspired by classic quantitative finance literature and modern backtesting frameworks
- Built with modern C++ best practices and clean architecture principles

---

**Note**: This is a backtesting framework for educational and research purposes. Past performance does not guarantee future results. Always test strategies thoroughly before live trading.
