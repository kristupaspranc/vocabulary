#pragma once

#include <iostream>

#include <sqlite3.h>

class DatabaseUtils{
protected:
    int dbCode;
    sqlite3 * dbHandle;
    char * errMsg = 0;

    void checkError();

public:
    DatabaseUtils() = default;
    DatabaseUtils(const DatabaseUtils& db) = delete;
    DatabaseUtils& operator= (const DatabaseUtils& db) = delete;

    ~DatabaseUtils();
};
