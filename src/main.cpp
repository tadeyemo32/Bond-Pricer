#include <iostream>
#include <iomanip>
#include <string>
#include <map>
#include <limits>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <ctime>
#include <chrono>
#include <vector>
#include "bond.h"
#include "db.h"
#include "market_data.h"

using namespace Bonds;

bool isRunning = true;

std::map<std::string, double> fxRates = {
    {"USD", 1.0},
    {"EUR", 0.92},
    {"GBP", 0.78},
    {"JPY", 145.0}
};

void line() { std::cout << "=============================================\n"; }

void printHeader(const std::string& title) {
    line();
    std::cout << "[INFO] " << title << "\n";
    line();
}

void printMenu() {
    printHeader("Quant Fixed-Income Toolkit");
    std::cout << "1. Price Zero-Coupon Bond (Price, Dur/Conv via r)\n";
    std::cout << "2. Price Coupon Bond (Price, Current Yield, Dur/Conv)\n";
    std::cout << "3. Search Bond DB\n";
    std::cout << "4. List All Bonds\n";
    std::cout << "5. Fetch Live Market Data\n";
    std::cout << "6. Quantitative Analysis Tools\n";
    std::cout << "7. Exit\n";
    line();
    std::cout << "Select option: ";
}

std::string getCurrency() {
    std::string currency;
    std::cout << "[INPUT] Currency (USD/EUR/GBP/JPY): ";
    std::cin >> currency;
    if (fxRates.find(currency) == fxRates.end()) return "USD";
    return currency;
}

bool askYesNo(const std::string& prompt) {
    std::cout << prompt << " (y/n): ";
    char ch; std::cin >> ch;
    return ch == 'y' || ch == 'Y';
}

double fetchMarketPrice(double modelPrice) {
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    return modelPrice * (0.95 + 0.1 * (rand() / double(RAND_MAX)));
}

void displayMarketDataMenu() {
    printHeader("Live Market Data");
    std::cout << "1. Fetch Stock Data\n";
    std::cout << "2. Fetch Bond Data\n";
    std::cout << "3. Back to Main Menu\n";
    line();
    std::cout << "Select option: ";
}

void displayQuantitativeMenu() {
    printHeader("Quantitative Analysis Tools");
    std::cout << "1. Fetch Live Market Data for Single Security\n";
    std::cout << "2. Compare Multiple Stocks\n";
    std::cout << "3. Compare Multiple Bonds\n";
    std::cout << "4. Volatility Analysis\n";
    std::cout << "5. View Market Data Log\n";
    std::cout << "6. Back to Main Menu\n";
    line();
    std::cout << "Select option: ";
}

void displayStockData(const std::map<std::string, double>& data, const std::string& symbol, const std::string& currency) {
    if (data.empty()) {
        std::cout << "[ERROR] Failed to fetch market data for " << symbol << "\n";
        std::cout << "[INFO] Make sure you have a valid API key in .env file\n";
        return;
    }
    
    if (data.find("open") == data.end()) {
        std::cout << "[ERROR] Invalid data format for " << symbol << "\n";
        return;
    }
    
    printHeader("Live Stock Data - " + symbol);
    std::cout << std::setprecision(2);
    std::cout << "Open:      " << data.at("open") * fxRates[currency] << " " << currency << "\n";
    std::cout << "High:      " << data.at("high") * fxRates[currency] << " " << currency << "\n";
    std::cout << "Low:       " << data.at("low") * fxRates[currency] << " " << currency << "\n";
    std::cout << "Close:     " << data.at("close") * fxRates[currency] << " " << currency << "\n";
    std::cout << "Volume:    " << data.at("volume") << " shares\n";
    std::cout << "Fetch Time:" << data.at("fetch_time") << " seconds\n";
    line();
}

void displayBondData(const std::map<std::string, double>& data, const std::string& symbol, const std::string& currency) {
    if (data.empty()) {
        std::cout << "[ERROR] Failed to fetch market data for " << symbol << "\n";
        std::cout << "[INFO] Make sure you have a valid API key in .env file\n";
        return;
    }
    
    if (data.find("price") == data.end()) {
        std::cout << "[ERROR] Invalid data format for " << symbol << "\n";
        return;
    }
    
    printHeader("Live Bond Data - " + symbol);
    std::cout << std::setprecision(2);
    std::cout << "Price:           " << data.at("price") * fxRates[currency] << " " << currency << "\n";
    std::cout << "Volume:          " << data.at("volume") << "\n";
    std::cout << "Change:          " << data.at("change") * fxRates[currency] << " " << currency << "\n";
    std::cout << "Change %:        " << data.at("change_percent") << " %\n";
    std::cout << "Fetch Time:      " << data.at("fetch_time") << " seconds\n";
    line();
}

void fetchAndAnalyzeSecurity() {
    std::string type, symbol, currency;
    std::cout << "[INPUT] Type (STOCK/BOND): ";
    std::cin >> type;
    std::cout << "[INPUT] Symbol: ";
    std::cin >> symbol;
    currency = getCurrency();
    
    std::cout << "[INFO] Fetching live data for " << symbol << "...\n";
    
    MarketDataResult result;
    if (type == "STOCK") {
        result = MarketData::fetchStockData(symbol);
    } else {
        result = MarketData::fetchBondData(symbol);
    }
    
    if (result.success) {
        printHeader("Live Market Data - " + symbol);
        std::time_t timestamp = std::chrono::system_clock::to_time_t(result.timestamp);
        std::cout << "[TIME] " << std::ctime(&timestamp);
        std::cout << "Fetch Time: " << result.fetch_time_ms << " ms\n";
        
        for (const auto& [key, value] : result.data) {
            std::cout << key << ": " << value;
            if (key.find("price") != std::string::npos || key.find("close") != std::string::npos) {
                std::cout << " " << currency << " (FX: " << value * fxRates[currency] << " " << currency << ")";
            }
            std::cout << "\n";
        }
        
        // Ask if user wants to compare with calculated price
        if (askYesNo("Compare with calculated price?")) {
            double calculatedPrice;
            std::cout << "[INPUT] Enter calculated price: ";
            std::cin >> calculatedPrice;
            
            double marketPrice = result.data.count("price") ? result.data["price"] :
                               result.data.count("close_1") ? result.data["close_1"] : 0;
            
            if (marketPrice > 0) {
                MarketData::compareWithCalculatedPrice(marketPrice, calculatedPrice, symbol);
            }
        }
    } else {
        std::cout << "[ERROR] " << result.error_message << "\n";
    }
}

void volatilityAnalysis() {
    std::string symbol;
    std::cout << "[INPUT] Stock Symbol for volatility analysis: ";
    std::cin >> symbol;
    
    auto result = MarketData::fetchStockData(symbol);
    if (result.success && result.data.count("annual_volatility")) {
        printHeader("Volatility Analysis - " + symbol);
        std::cout << "Annualized Volatility: " << result.data["annual_volatility"] * 100 << "%\n";
        
        // Volatility interpretation
        double vol = result.data["annual_volatility"];
        if (vol > 0.4) {
            std::cout << "Risk Level: \033[31mVERY HIGH\033[0m (Speculative)\n";
        } else if (vol > 0.25) {
            std::cout << "Risk Level: \033[33mHIGH\033[0m (Growth)\n";
        } else if (vol > 0.15) {
            std::cout << "Risk Level: \033[32mMODERATE\033[0m (Balanced)\n";
        } else {
            std::cout << "Risk Level: \033[34mLOW\033[0m (Conservative)\n";
        }
    } else {
        std::cout << "[ERROR] Could not fetch volatility data for " << symbol << "\n";
    }
}

void compareMultipleSecurities() {
    std::string type;
    std::cout << "[INPUT] Type to compare (STOCK/BOND): ";
    std::cin >> type;
    
    std::vector<std::string> symbols;
    std::cout << "[INPUT] Enter symbols (space separated, type 'done' when finished):\n";
    std::string symbol;
    while (std::cin >> symbol && symbol != "done") {
        symbols.push_back(symbol);
    }
    
    if (symbols.empty()) return;
    
    printHeader("Comparing " + std::to_string(symbols.size()) + " " + type + " Securities");
    
    for (const auto& symbol : symbols) {
        std::cout << "\n--- " << symbol << " ---\n";
        MarketDataResult result;
        if (type == "STOCK") {
            result = MarketData::fetchStockData(symbol);
        } else {
            result = MarketData::fetchBondData(symbol);
        }
        
        if (result.success) {
            if (result.data.count("price")) {
                std::cout << "Price: " << result.data.at("price") << "\n";
            }
            if (result.data.count("change_percent")) {
                double change = result.data.at("change_percent");
                std::cout << "Change: ";
                if (change >= 0) {
                    std::cout << "\033[32m+" << change << "%\033[0m\n";
                } else {
                    std::cout << "\033[31m" << change << "%\033[0m\n";
                }
            }
            if (result.data.count("annual_volatility")) {
                std::cout << "Volatility: " << result.data.at("annual_volatility") * 100 << "%\n";
            }
        } else {
            std::cout << "Error: " << result.error_message << "\n";
        }
    }
}

int main(int, char**) {
    std::srand(std::time(nullptr));
    std::cout.setf(std::ios::fixed);
    std::cout << std::setprecision(2);

    BondDB db("bonds.db");
    db.init();

    while (isRunning) {
        printMenu();
        int choice;
        if (!(std::cin >> choice)) return 1;

        if (choice == 1) {
            std::string cur = getCurrency();
            double FV, r; int T;
            std::string bondName;
            std::cin.ignore();
            std::cout << "[INPUT] Name of the Bond: ";
            std::getline(std::cin, bondName);
            std::cout << "[INPUT] Face Value (" << cur << "): "; std::cin >> FV;
            std::cout << "[INPUT] Discount Rate (%): "; std::cin >> r;
            std::cout << "[INPUT] Maturity (years): "; std::cin >> T;

            if (FV <= 0 || r < 0 || T <= 0) continue;

            zc_Bond zc(FV, r/100.0, T);
            double priceUSD = zc.price();
            double priceCur = priceUSD * fxRates[cur];

            printHeader("Zero-Coupon Bond — Results");
            std::cout << std::setprecision(4);
            std::cout << "Bond Name           : " << bondName << "\n";
            std::cout << "Face Value          : " << FV << " " << cur << "\n";
            std::cout << "Discount Rate (ann) : " << r  << " %\n";
            std::cout << "Maturity            : " << T  << " years\n";
            std::cout << "Price               : " << std::setprecision(2) << priceCur << " " << cur << "\n";
            std::cout << std::setprecision(6);
            std::cout << "Macaulay Duration   : " << zc.macaulay_duration() << " years\n";
            std::cout << "Modified Duration   : " << zc.modified_duration() << " years\n";
            std::cout << "Convexity           : " << zc.convexity() << " years^2\n\n";

            if (askYesNo("Save bond to DB?")) {
                db.saveBond(bondName, "Zero-Coupon", FV, 0.0, r/100.0, T, 1, priceUSD, cur);
            }

            if (askYesNo("Fetch simulated market price?")) {
                double mktPrice = fetchMarketPrice(priceUSD);
                double mktCur = mktPrice * fxRates[cur];
                std::cout << "\033[32m[MARKET] Price: " << std::setprecision(2) << mktCur << " " << cur << "\033[0m\n";
                double y = zc.ytm(mktPrice);
                if (std::isfinite(y)) std::cout << "[MARKET] Implied YTM: " << y*100 << " %\n\n";
            }

            if (askYesNo("Fetch live market comparison?")) {
                std::string marketSymbol;
                std::cout << "[INPUT] Market symbol for comparison: ";
                std::cin >> marketSymbol;
                
                auto marketResult = MarketData::fetchBondData(marketSymbol);
                if (marketResult.success && marketResult.data.count("price")) {
                    double marketPrice = marketResult.data["price"];
                    MarketData::compareWithCalculatedPrice(marketPrice, priceUSD, marketSymbol);
                    MarketData::analyzePriceDiscrepancy(marketPrice, priceUSD,
                                                      zc.modified_duration(),
                                                      zc.convexity(),
                                                      zc.ytm(marketPrice));
                }
            }
        }
        else if (choice == 2) {
            std::string cur = getCurrency();
            double FV, c, r; int T, freq;
            std::string bondName;
            std::cin.ignore();
            std::cout << "[INPUT] Name of the Bond: ";
            std::getline(std::cin, bondName);
            std::cout << "[INPUT] Face Value (" << cur << "): "; std::cin >> FV;
            std::cout << "[INPUT] Coupon Rate (% annual): "; std::cin >> c;
            std::cout << "[INPUT] Discount Rate (% annual): "; std::cin >> r;
            std::cout << "[INPUT] Maturity (years): "; std::cin >> T;
            std::cout << "[INPUT] Frequency (1=annual,2=semi,4=q): "; std::cin >> freq;

            if (FV <= 0 || c < 0 || r < 0 || T <= 0 || freq <= 0) continue;

            c_Bond bond(FV, c/100.0, r/100.0, T, freq);
            double priceUSD = bond.price();
            double priceCur = priceUSD * fxRates[cur];

            printHeader("Coupon Bond — Results");
            std::cout << std::setprecision(4);
            std::cout << "Bond Name           : " << bondName << "\n";
            std::cout << "Face Value          : " << FV << " " << cur << "\n";
            std::cout << "Coupon Rate (ann)   : " << c  << " %\n";
            std::cout << "Discount Rate (ann) : " << r  << " %\n";
            std::cout << "Maturity            : " << T  << " years\n";
            std::cout << "Frequency           : " << freq << " / year\n";
            std::cout << "Price               : " << std::setprecision(2) << priceCur << " " << cur << "\n";
            std::cout << std::setprecision(6);
            std::cout << "Current Yield       : " << bond.current_yield(priceUSD)*100.0 << " %\n";
            std::cout << "Macaulay Duration   : " << bond.macaulay_duration() << " years\n";
            std::cout << "Modified Duration   : " << bond.modified_duration() << " years\n";
            std::cout << "Convexity           : " << bond.convexity() << " years^2\n\n";

            if (askYesNo("Save bond to DB?")) {
                db.saveBond(bondName, "Coupon", FV, c/100.0, r/100.0, T, freq, priceUSD, cur);
            }

            if (askYesNo("Fetch simulated market price?")) {
                double mktPrice = fetchMarketPrice(priceUSD);
                double mktCur = mktPrice * fxRates[cur];
                std::cout << "\033[32m[MARKET] Price: " << std::setprecision(2) << mktCur << " " << cur << "\033[0m\n";
                double y = bond.ytm(mktPrice);
                if (std::isfinite(y)) std::cout << "[MARKET] Implied YTM: " << y*100 << " %\n\n";
            }

            if (askYesNo("Fetch live market comparison?")) {
                std::string marketSymbol;
                std::cout << "[INPUT] Market symbol for comparison: ";
                std::cin >> marketSymbol;
                
                auto marketResult = MarketData::fetchBondData(marketSymbol);
                if (marketResult.success && marketResult.data.count("price")) {
                    double marketPrice = marketResult.data["price"];
                    MarketData::compareWithCalculatedPrice(marketPrice, priceUSD, marketSymbol);
                    MarketData::analyzePriceDiscrepancy(marketPrice, priceUSD,
                                                      bond.modified_duration(),
                                                      bond.convexity(),
                                                      bond.ytm(marketPrice));
                }
            }
        }
        else if (choice == 3) {
            std::cin.ignore();
            std::string name;
            std::cout << "[INPUT] Bond Name to search: ";
            std::getline(std::cin, name);
            auto results = db.searchBond(name);
            printHeader("Search Results");
            if (results.empty()) {
                std::cout << "No bonds found matching: " << name << "\n";
            } else {
                for (const auto& bond : results) {
                    std::cout << "- " << bond << "\n";
                }
            }
            line();
        }
        else if (choice == 4) {
            auto results = db.listBonds();
            printHeader("All Bonds in Database");
            if (results.empty()) {
                std::cout << "No bonds in database.\n";
            } else {
                for (const auto& bond : results) {
                    std::cout << "- " << bond << "\n";
                }
            }
            line();
        }
        else if (choice == 5) {
            bool marketMenuRunning = true;
            while (marketMenuRunning) {
                displayMarketDataMenu();
                int marketChoice;
                if (!(std::cin >> marketChoice)) break;
                
                if (marketChoice == 1) {
                    std::string symbol, currency;
                    std::cout << "[INPUT] Stock Symbol (e.g., AAPL): ";
                    std::cin >> symbol;
                    currency = getCurrency();
                    
                    std::cout << "[INFO] Fetching live data for " << symbol << "...\n";
                    auto data = MarketData::fetchStockData(symbol);
                    if (data.success) {
                        displayStockData(data.data, symbol, currency);
                    } else {
                        std::cout << "[ERROR] " << data.error_message << "\n";
                    }
                }
                else if (marketChoice == 2) {
                    std::string symbol, currency;
                    std::cout << "[INPUT] Bond Symbol (e.g., US10Y): ";
                    std::cin >> symbol;
                    currency = getCurrency();
                    
                    std::cout << "[INFO] Fetching live data for " << symbol << "...\n";
                    auto data = MarketData::fetchBondData(symbol);
                    if (data.success) {
                        displayBondData(data.data, symbol, currency);
                    } else {
                        std::cout << "[ERROR] " << data.error_message << "\n";
                    }
                }
                else if (marketChoice == 3) {
                    marketMenuRunning = false;
                }
                else {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                }
            }
        }
        else if (choice == 6) {
            bool quantMenuRunning = true;
            while (quantMenuRunning) {
                displayQuantitativeMenu();
                int quantChoice;
                if (!(std::cin >> quantChoice)) break;
                
                switch(quantChoice) {
                    case 1: fetchAndAnalyzeSecurity(); break;
                    case 2: compareMultipleSecurities(); break;
                    case 3: compareMultipleSecurities(); break;
                    case 4: volatilityAnalysis(); break;
                    case 5:
                        printHeader("Market Data Log");
                        std::cout << "Log feature coming soon...\n";
                        break;
                    case 6: quantMenuRunning = false; break;
                    default:
                        std::cout << "[ERROR] Invalid option\n";
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                }
            }
        }
        else if (choice == 7) {
            isRunning = false;
            printHeader("Exiting Quant Fixed-Income Toolkit");
            std::cout << "Thank you for using the system!\n";
            line();
        }
        else {
            if (!std::cin) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
        }
    }
    return 0;
}
