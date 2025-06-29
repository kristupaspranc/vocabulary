#pragma once

#include <fstream>

#include <sqlite3.h>


class DatabaseUtils{
private:
    static std::size_t s_referenceCount;

private:
    void acquireReference();
    void releaseReference();

protected:
    int m_dbCode;
    sqlite3 * m_dbHandle;
    char * m_errMsg = 0;

    static std::ofstream s_log;

protected:
    void checkSQLError();

public:
    DatabaseUtils();
    // Copy and move constructors can only be enabled if the databases are closed
    // and opened manually in each as SQLite3 has inner dependencies to the
    // outside pointer and putting it to new owner ship does not work
    DatabaseUtils(const DatabaseUtils& db) = delete;
    DatabaseUtils& operator= (const DatabaseUtils& db) = delete;
    DatabaseUtils(DatabaseUtils&& db) = delete;
    DatabaseUtils& operator= (DatabaseUtils&& db) = delete;
    ~DatabaseUtils();
};
