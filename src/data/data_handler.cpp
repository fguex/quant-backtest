#include "data_handler.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

namespace backtest {
    DataHandler::DataHandler() : current_index(0) {}

    bool DataHandler::load_csv(const std::string& file_path) {
        std::ifstream file(file_path);
        if (!file.is_open()) {
            std::cerr << "Error opening file: " << file_path << std::endl;
            return false;
        }

        std::string line;
        // Skip header
        std::getline(file, line);

        while (std::getline(file, line)) {
            std::istringstream ss(line);
            std::string timestamp;
            double open, high, low, close, volume;

            std::getline(ss, timestamp, ',');
            ss >> open; ss.ignore();
            ss >> high; ss.ignore();
            ss >> low; ss.ignore();
            ss >> close; ss.ignore();
            ss >> volume;

            bars_.emplace_back(timestamp, open, high, low, close, volume);
        }

        file.close();
        return true;
    }

    bool DataHandler::has_next() const {
        return current_index < bars_.size();
    }

    Bar DataHandler::get_next_bar() {
        if (!has_next()) {
            throw std::out_of_range("No more bars available");
        }
        return bars_[current_index++];
    }

    void DataHandler::reset() {
        current_index = 0;
    }
} // namespace backtest