#include<filesystem>
#include<iostream>

#include "DatabaseUtils.h"


void DatabaseUtils::checkError(){
    if (dbCode != SQLITE_OK){
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }
}

DatabaseUtils::~DatabaseUtils(){
    if (dbCode){
        std::cout << "DB Error: " << sqlite3_errmsg(dbHandle) << "\n";
        sqlite3_close(dbHandle);
    }

    sqlite3_close(dbHandle);
}

bool DatabaseUtils::checkDatabaseExistence(std::string dbName){
    dbName = "Databases/" + dbName + ".db";
    return std::filesystem::exists(dbName);
}
