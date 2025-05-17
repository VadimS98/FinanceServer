#pragma once
#include <sqlite3.h>
#include <string>

class DBManager {
private:
	sqlite3* db = nullptr;
	std::string db_path;

public:
	DBManager(const std::string& path);
	~DBManager(); 

	bool initialize();
	sqlite3* getDB() const;
};
