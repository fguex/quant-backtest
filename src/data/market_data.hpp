#ifndef MARKET_DATA_HPP
#define MARKET_DATA_HPP
#include <string>

namespace backtest {

struct Bar {
    std::string timestamp; // ISO 8601 format
    double open;
    double high;
    double low;
    double close;
    double volume;

    Bar(const std::string& ts, double o, double h, double l, double c, double v)
        : timestamp(ts), open(o), high(h), low(l), close(c), volume(v) {}

    Bar() : timestamp(""), open(0), high(0), low(0), close(0), volume(0) {}
    
};

} // namespace backtest
#endif // MARKET_DATA_HPP

