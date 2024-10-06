#include<filesystem>

#include "DatabaseUtils.h"

DatabaseUtils::DatabaseUtils(){
    m_log.open("log.txt");
}

void DatabaseUtils::checkSQLError(){
    if (m_dbCode != SQLITE_OK && m_dbCode != SQLITE_CONSTRAINT){
        m_log << "SQL error: " <<  m_errMsg << "\n";
        sqlite3_free(m_errMsg);
    }
}

DatabaseUtils::~DatabaseUtils(){
    if (m_dbCode != SQLITE_OK && m_dbCode != SQLITE_CONSTRAINT){
        m_log << "DB Error: " << sqlite3_errmsg(m_dbHandle) << "\n";
        sqlite3_close(m_dbHandle);
    }

    sqlite3_close(m_dbHandle);
    m_log.close();
}

bool DatabaseUtils::s_checkDatabaseExistence(std::string dbName){
    dbName = "Databases/" + dbName + ".db";
    return std::filesystem::exists(dbName);
}
