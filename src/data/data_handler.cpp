/**
 * @file data_handler.cpp
 * @brief Implementation of CSV data loading and iteration functionality
 */

#include "data_handler.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

namespace backtest {
    
    /**
     * @brief Initializes an empty DataHandler with index at position 0
     */
    DataHandler::DataHandler() : current_index(0) {}

    /**
     * @brief Loads historical market data from CSV file
     * @param file_path Path to CSV file with OHLCV data
     * @return true if successful, false if file cannot be opened
     * 
     * CSV Format: timestamp,open,high,low,close,volume
     * - First row (header) is automatically skipped
     * - Comma-separated values
     * - Timestamp should be a string (e.g., "2024-01-15")
     * - All prices and volume are parsed as doubles
     */
    bool DataHandler::load_csv(const std::string& file_path) {
        std::ifstream file(file_path);
        if (!file.is_open()) {
            std::cerr << "Error opening file: " << file_path << std::endl;
            return false;
        }

        std::string line;
        // Skip header row
        std::getline(file, line);

        // Parse each data row
        while (std::getline(file, line)) {
            std::istringstream ss(line);
            std::string timestamp;
            double open, high, low, close, volume;

            // Parse CSV fields
            std::getline(ss, timestamp, ',');  // Read timestamp until comma
            ss >> open; ss.ignore();            // Read open, skip comma
            ss >> high; ss.ignore();            // Read high, skip comma
            ss >> low; ss.ignore();             // Read low, skip comma
            ss >> close; ss.ignore();           // Read close, skip comma
            ss >> volume;                       // Read volume (last field)

            // Create Bar and add to collection
            bars_.emplace_back(timestamp, open, high, low, close, volume);
        }

        file.close();
        return true;
    }

    /**
     * @brief Checks if there are more bars to process
     * @return true if current_index is within bounds
     */
    bool DataHandler::has_next() const {
        return current_index < bars_.size();
    }

    /**
     * @brief Returns the next bar and increments the index
     * @return The Bar at current_index
     * @throws std::out_of_range if no more bars are available
     */
    Bar DataHandler::get_next_bar() {
        if (!has_next()) {
            throw std::out_of_range("No more bars available");
        }
        return bars_[current_index++];
    }

    /**
     * @brief Resets the data iterator to the beginning
     * Allows re-running backtests on the same dataset
     */
    void DataHandler::reset() {
        current_index = 0;
    }
    
} // namespace backtest