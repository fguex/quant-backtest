/**
 * @file data_handler.hpp
 * @brief Data loading and iteration management for backtesting
 * 
 * Provides functionality to load historical market data from CSV files
 * and iterate through it sequentially for backtesting strategies.
 */

#ifndef DATA_HANDLER_HPP
#define DATA_HANDLER_HPP
#include <string>
#include <vector>
#include "market_data.hpp"

namespace backtest {
    
    /**
     * @class DataHandler
     * @brief Manages loading and sequential access to historical market data
     * 
     * Loads OHLCV data from CSV files and provides an iterator-like interface
     * to access bars sequentially, simulating real-time data feed during backtesting.
     */
    class DataHandler {
        private: 
            std::vector<Bar> bars_;        ///< Storage for all loaded price bars
            size_t current_index;          ///< Current position in the data sequence
            
        public:
            /**
             * @brief Default constructor - initializes empty data handler
             */
            DataHandler();

            /**
             * @brief Loads historical market data from a CSV file
             * @param file_path Path to the CSV file containing OHLCV data
             * @return true if file loaded successfully, false otherwise
             * 
             * Expected CSV format: timestamp,open,high,low,close,volume
             * First line (header) is skipped automatically.
             */
            bool load_csv(const std::string& file_path);
            
            /**
             * @brief Checks if more data bars are available
             * @return true if there are unprocessed bars remaining
             */
            bool has_next() const;
            
            /**
             * @brief Retrieves the next bar and advances the iterator
             * @return Next Bar in the sequence
             * @throws std::out_of_range if no more bars are available
             */
            Bar get_next_bar();
            
            /**
             * @brief Resets the iterator to the beginning of the data
             */
            void reset();

            /**
             * @brief Returns the total number of loaded bars
             * @return Total count of bars in the dataset
             */
            size_t size() const { return bars_.size(); }
    };
    
} // namespace backtest
#endif // DATA_HANDLER_HPP  