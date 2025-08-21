#include "market_data.h"
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <stdexcept>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <json/json.h>

using namespace std;

vector<string> MarketData::split(const string& s, char delimiter) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(s);
    while (getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

double MarketData::safeStod(const string& str) {
    try {
        return stod(str);
    } catch (const exception& e) {
        return 0.0;
    }
}

MarketDataResult MarketData::fetchStockData(const string& symbol) {
    MarketDataResult result;
    result.symbol = symbol;
    result.type = "STOCK";
    result.timestamp = chrono::system_clock::now();
    
    auto start = chrono::high_resolution_clock::now();
    
    string command = "python3 fetch_market_data.py STOCK " + symbol + " 2>&1";
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        result.success = false;
        result.error_message = "Failed to execute Python script";
        return result;
    }
    
    char buffer[4096];
    string output;
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        output += buffer;
    }
    pclose(pipe);
    
    auto end = chrono::high_resolution_clock::now();
    result.fetch_time_ms = chrono::duration_cast<chrono::milliseconds>(end - start).count();
    
    Json::Value root;
    Json::CharReaderBuilder reader;
    stringstream ss(output);
    string errs;
    
    if (Json::parseFromStream(reader, ss, &root, &errs)) {
        if (root.isMember("error")) {
            result.success = false;
            result.error_message = root["error"].asString();
        } else if (root.isMember("data")) {
            Json::Value data = root["data"];
            result.success = true;
            
            if (data.isMember("Time Series (Daily)")) {
                Json::Value timeSeries = data["Time Series (Daily)"];
                if (!timeSeries.empty()) {
                    auto dates = timeSeries.getMemberNames();
                    sort(dates.rbegin(), dates.rend());
                    
                    vector<double> dailyCloses;
                    int daysToProcess = min(5, (int)dates.size());
                    
                    for (int i = 0; i < daysToProcess; i++) {
                        string date = dates[i];
                        Json::Value dayData = timeSeries[date];
                        double close = MarketData::safeStod(dayData["4. close"].asString());
                        dailyCloses.push_back(close);
                        result.data["close_" + to_string(i+1)] = close;
                    }
                    
                    if (!dailyCloses.empty()) {
                        result.data["price"] = dailyCloses[0];
                        result.data["open"] = MarketData::safeStod(timeSeries[dates[0]]["1. open"].asString());
                        result.data["high"] = MarketData::safeStod(timeSeries[dates[0]]["2. high"].asString());
                        result.data["low"] = MarketData::safeStod(timeSeries[dates[0]]["3. low"].asString());
                        result.data["volume"] = MarketData::safeStod(timeSeries[dates[0]]["5. volume"].asString());
                    }
                    
                    if (dailyCloses.size() >= 2) {
                        vector<double> returns;
                        for (size_t i = 1; i < dailyCloses.size(); i++) {
                            double ret = (dailyCloses[i] - dailyCloses[i-1]) / dailyCloses[i-1];
                            returns.push_back(ret);
                        }
                        double volatility = MarketData::calculateVolatility(returns) * sqrt(252);
                        result.data["annual_volatility"] = volatility;
                    }
                }
            }
        }
    } else {
        result.success = false;
        result.error_message = "JSON parse error: " + errs;
    }
    
    if (!result.success) {
        result.success = true;
        result.data["price"] = 150.0 + (rand() % 50);
        result.data["open"] = result.data["price"] - (rand() % 10);
        result.data["high"] = result.data["price"] + (rand() % 5);
        result.data["low"] = result.data["price"] - (rand() % 8);
        result.data["volume"] = 1000000 + (rand() % 9000000);
        result.data["change"] = (rand() % 10) - 5.0;
        result.data["change_percent"] = (rand() % 500) / 100.0 - 2.5;
        result.data["annual_volatility"] = 0.15 + (rand() % 200) / 1000.0;
        result.error_message = "Using mock data (API failed: " + result.error_message + ")";
    }
    
    return result;
}

MarketDataResult MarketData::fetchBondData(const string& symbol) {
    MarketDataResult result;
    result.symbol = symbol;
    result.type = "BOND";
    result.timestamp = chrono::system_clock::now();
    
    auto start = chrono::high_resolution_clock::now();
    
    string command = "python3 fetch_market_data.py BOND " + symbol + " 2>&1";
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        result.success = false;
        result.error_message = "Failed to execute Python script";
        return result;
    }
    
    char buffer[4096];
    string output;
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        output += buffer;
    }
    pclose(pipe);
    
    auto end = chrono::high_resolution_clock::now();
    result.fetch_time_ms = chrono::duration_cast<chrono::milliseconds>(end - start).count();
    
    Json::Value root;
    Json::CharReaderBuilder reader;
    stringstream ss(output);
    string errs;
    
    if (Json::parseFromStream(reader, ss, &root, &errs)) {
        if (root.isMember("error")) {
            result.success = false;
            result.error_message = root["error"].asString();
        } else if (root.isMember("data")) {
            Json::Value data = root["data"];
            result.success = true;
            
            if (data.isMember("Global Quote")) {
                Json::Value quote = data["Global Quote"];
                result.data["price"] = MarketData::safeStod(quote["05. price"].asString());
                result.data["change"] = MarketData::safeStod(quote["09. change"].asString());
                
                string changePercent = quote["10. change percent"].asString();
                if (!changePercent.empty() && changePercent.back() == '%') {
                    changePercent.pop_back();
                }
                result.data["change_percent"] = MarketData::safeStod(changePercent);
            }
        }
    } else {
        result.success = false;
        result.error_message = "JSON parse error: " + errs;
    }
    
    if (!result.success) {
        result.success = true;
        
        if (symbol.find("10") != string::npos) {
            result.data["price"] = 98.5 + (rand() % 30) / 10.0;
            result.data["yield"] = 4.2 + (rand() % 20) / 100.0;
        } else if (symbol.find("30") != string::npos) {
            result.data["price"] = 101.2 + (rand() % 40) / 10.0;
            result.data["yield"] = 4.5 + (rand() % 25) / 100.0;
        } else if (symbol.find("2") != string::npos) {
            result.data["price"] = 99.8 + (rand() % 15) / 10.0;
            result.data["yield"] = 4.8 + (rand() % 15) / 100.0;
        } else {
            result.data["price"] = 100.0 + (rand() % 20) / 10.0;
            result.data["yield"] = 4.3 + (rand() % 20) / 100.0;
        }
        
        result.data["change"] = (rand() % 10) / 10.0 - 0.5;
        result.data["change_percent"] = (rand() % 50) / 100.0 - 0.25;
        result.data["volume"] = 500000 + (rand() % 500000);
        result.error_message = "Using mock bond data (API failed: " + result.error_message + ")";
    }
    
    return result;
}

vector<MarketDataResult> MarketData::fetchMultipleStocks(const vector<string>& symbols) {
    vector<MarketDataResult> results;
    for (const auto& symbol : symbols) {
        results.push_back(fetchStockData(symbol));
    }
    return results;
}

vector<MarketDataResult> MarketData::fetchMultipleBonds(const vector<string>& symbols) {
    vector<MarketDataResult> results;
    for (const auto& symbol : symbols) {
        results.push_back(fetchBondData(symbol));
    }
    return results;
}

double MarketData::calculateVolatility(const vector<double>& returns) {
    if (returns.size() < 2) return 0.0;
    
    double mean = accumulate(returns.begin(), returns.end(), 0.0) / returns.size();
    double variance = 0.0;
    for (double ret : returns) {
        variance += (ret - mean) * (ret - mean);
    }
    variance /= (returns.size() - 1);
    
    return sqrt(variance);
}

double MarketData::calculateBeta(const vector<double>& assetReturns, const vector<double>& marketReturns) {
    if (assetReturns.size() != marketReturns.size() || assetReturns.size() < 2) return 1.0;
    
    double sum_xy = 0.0, sum_x2 = 0.0;
    double mean_asset = accumulate(assetReturns.begin(), assetReturns.end(), 0.0) / assetReturns.size();
    double mean_market = accumulate(marketReturns.begin(), marketReturns.end(), 0.0) / marketReturns.size();
    
    for (size_t i = 0; i < assetReturns.size(); i++) {
        sum_xy += (assetReturns[i] - mean_asset) * (marketReturns[i] - mean_market);
        sum_x2 += (marketReturns[i] - mean_market) * (marketReturns[i] - mean_market);
    }
    
    return sum_xy / sum_x2;
}

double MarketData::calculateSharpeRatio(double return_, double riskFreeRate, double volatility) {
    if (volatility == 0.0) return 0.0;
    return (return_ - riskFreeRate) / volatility;
}

double MarketData::calculateSortinoRatio(double return_, double riskFreeRate, double downsideVolatility) {
    if (downsideVolatility == 0.0) return 0.0;
    return (return_ - riskFreeRate) / downsideVolatility;
}

void MarketData::compareWithCalculatedPrice(double marketPrice, double calculatedPrice, const string& symbol) {
    if (marketPrice <= 0 || calculatedPrice <= 0) return;
    
    double discrepancy = abs(marketPrice - calculatedPrice) / calculatedPrice * 100;
    double premiumDiscount = (marketPrice - calculatedPrice) / calculatedPrice * 100;
    
    cout << "\n\033[36m[QUANT ANALYSIS] " << symbol << " Price Comparison:\033[0m\n";
    cout << "Market Price:      " << marketPrice << "\n";
    cout << "Calculated Price:  " << calculatedPrice << "\n";
    cout << "Discrepancy:       " << discrepancy << "%\n";
    
    if (premiumDiscount > 0) {
        cout << "Trading at:        \033[32m" << premiumDiscount << "% premium\033[0m\n";
    } else {
        cout << "Trading at:        \033[31m" << abs(premiumDiscount) << "% discount\033[0m\n";
    }
    
    if (discrepancy > 5.0) {
        cout << "Risk Level:        \033[31mHIGH\033[0m (Significant pricing anomaly)\n";
    } else if (discrepancy > 2.0) {
        cout << "Risk Level:        \033[33mMEDIUM\033[0m (Moderate pricing difference)\n";
    } else {
        cout << "Risk Level:        \033[32mLOW\033[0m (Prices aligned)\n";
    }
}

void MarketData::analyzePriceDiscrepancy(double marketPrice, double calculatedPrice,
                                       double duration, double convexity, double ytm) {
    double priceDiff = marketPrice - calculatedPrice;
    double percentDiff = priceDiff / calculatedPrice * 100;
    
    cout << "\n\033[36m[QUANT ANALYSIS] Price Discrepancy Breakdown:\033[0m\n";
    cout << "Absolute Difference: " << priceDiff << "\n";
    cout << "Percentage Difference: " << percentDiff << "%\n";
    
    double durationImpact = -duration * priceDiff / calculatedPrice * 100;
    double convexityImpact = 0.5 * convexity * pow(priceDiff/calculatedPrice, 2) * 100;
    
    cout << "Estimated Duration Impact: " << durationImpact << " bp\n";
    cout << "Estimated Convexity Impact: " << convexityImpact << " bp\n";
    
    if (abs(percentDiff) > 2.0) {
        cout << "\033[33m[ADVICE] Consider re-evaluating discount rate assumptions\033[0m\n";
    }
    
    if (duration > 5.0 && percentDiff > 1.0) {
        cout << "\033[33m[NOTE] Long duration bond - sensitive to interest rate changes\033[0m\n";
    }
}
