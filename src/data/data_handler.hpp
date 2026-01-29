#ifndef DATA_HANDLER_HPP
#define DATA_HANDLER_HPP
#include <string>
#include <vector>
#include "market_data.hpp"
namespace backtest {
    class DataHandler{
        private: 
            std::vector<Bar> bars_;
            size_t current_index;
        public:
            DataHandler();

            bool load_csv(const std::string& file_path);
            bool has_next() const;
            Bar get_next_bar();
            void reset();

            size_t size() const{return bars_.size();}
    };
} // namespace backtest
#endif // DATA_HANDLER_HPP  