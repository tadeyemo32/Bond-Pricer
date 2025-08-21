#ifndef MARKET_DATA_H
#define MARKET_DATA_H

#include <string>
#include <map>
#include <vector>
#include <chrono>

struct MarketDataResult {
    std::map<std::string, double> data;
    std::string symbol;
    std::string type;
    std::chrono::system_clock::time_point timestamp;
    bool success;
    std::string error_message;
    double fetch_time_ms;
};

class MarketData {
public:
    static MarketDataResult fetchStockData(const std::string& symbol);
    static MarketDataResult fetchBondData(const std::string& symbol);
    static std::vector<MarketDataResult> fetchMultipleStocks(const std::vector<std::string>& symbols);
    static std::vector<MarketDataResult> fetchMultipleBonds(const std::vector<std::string>& symbols);
    
    // Quantitative analysis functions
    static double calculateVolatility(const std::vector<double>& returns);
    static double calculateBeta(const std::vector<double>& assetReturns, const std::vector<double>& marketReturns);
    static double calculateSharpeRatio(double return_, double riskFreeRate, double volatility);
    static double calculateSortinoRatio(double return_, double riskFreeRate, double downsideVolatility);
    
    // Comparison functions
    static void compareWithCalculatedPrice(double marketPrice, double calculatedPrice, const std::string& symbol);
    static void analyzePriceDiscrepancy(double marketPrice, double calculatedPrice,
                                      double duration, double convexity, double ytm);
    
private:
    static std::vector<std::string> split(const std::string& s, char delimiter);
    static double safeStod(const std::string& str);
};

#endif
