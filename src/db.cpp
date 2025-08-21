#include "db.h"
#include <iostream>

BondDB::BondDB(const std::string& filename) : dbFile(filename), db(nullptr) {}
BondDB::~BondDB() { if(db) sqlite3_close(db); }

void BondDB::init() {
    sqlite3_open(dbFile.c_str(), &db);
    const char* sql="CREATE TABLE IF NOT EXISTS bonds("
        "id INTEGER PRIMARY KEY,"
        "name TEXT,"
        "type TEXT,"
        "FV REAL,"
        "c REAL,"
        "r REAL,"
        "T INTEGER,"
        "freq INTEGER,"
        "price REAL,"
        "currency TEXT);";
    sqlite3_exec(db, sql, nullptr, nullptr, nullptr);
}

void BondDB::saveBond(const std::string& name, const std::string& type,
                      double FV, double c, double r, int T, int freq,
                      double price, const std::string& currency) {
    std::string sql="INSERT INTO bonds(name,type,FV,c,r,T,freq,price,currency) VALUES('"
        +name+"','"+type+"',"+std::to_string(FV)+","+std::to_string(c)+","+std::to_string(r)+","+std::to_string(T)+","+std::to_string(freq)+","+std::to_string(price)+",'"+currency+"');";
    sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);
}

std::vector<std::string> BondDB::listBonds() {
    std::vector<std::string> res;
    const char* sql="SELECT name FROM bonds;";
    sqlite3_stmt* stmt;
    if(sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr)==SQLITE_OK){
        while(sqlite3_step(stmt)==SQLITE_ROW)
            res.push_back(reinterpret_cast<const char*>(sqlite3_column_text(stmt,0)));
        sqlite3_finalize(stmt);
    }
    return res;
}

std::vector<std::string> BondDB::searchBond(const std::string& name){
    std::vector<std::string> res;
    std::string sql="SELECT name FROM bonds WHERE name LIKE '%"+name+"%';";
    sqlite3_stmt* stmt;
    if(sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr)==SQLITE_OK){
        while(sqlite3_step(stmt)==SQLITE_ROW)
            res.push_back(reinterpret_cast<const char*>(sqlite3_column_text(stmt,0)));
        sqlite3_finalize(stmt);
    }
    return res;
}

