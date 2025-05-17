#include "db_manager.h"
#include <iostream>

DBManager::DBManager(const std::string& path) : db_path(path) {}

DBManager::~DBManager() {
    if (db) {
        sqlite3_close(db);
    } 
}

bool DBManager::initialize() {
    int rc = sqlite3_open(db_path.c_str(), &db);
    if (rc != SQLITE_OK) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    char* errorMessage = nullptr;
    rc = sqlite3_exec(db, "PRAGMA encoding = 'UTF-8';", nullptr, nullptr, &errorMessage);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to set encoding: " << errorMessage << std::endl;
        sqlite3_free(errorMessage);
        return false;
    }

    return true;
}

sqlite3* DBManager::getDB() const {
    return db;
}
