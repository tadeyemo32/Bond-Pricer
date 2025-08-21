#ifndef DB_H
#define DB_H

#include <string>
#include <vector>
#include <sqlite3.h>

class BondDB {
private:
    std::string dbFile;
    sqlite3* db;
public:
    BondDB(const std::string& filename);
    ~BondDB();
    void init();
    void saveBond(const std::string& name, const std::string& type,
                  double FV, double c, double r, int T, int freq,
                  double price, const std::string& currency);
    std::vector<std::string> listBonds();
    std::vector<std::string> searchBond(const std::string& name);
};
#endif

