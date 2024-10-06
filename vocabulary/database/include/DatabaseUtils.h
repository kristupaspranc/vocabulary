#pragma once

#include <fstream>
#include <string>

#include <sqlite3.h>


class DatabaseUtils{
protected:
    int m_dbCode;
    sqlite3 * m_dbHandle;
    char * m_errMsg = 0;

    std::ofstream m_log;

protected:
    void checkSQLError();

public:
    DatabaseUtils();
    DatabaseUtils(const DatabaseUtils& db) = delete;
    DatabaseUtils& operator= (const DatabaseUtils& db) = delete;
    ~DatabaseUtils();

    static bool s_checkDatabaseExistence(std::string dbName);
};
