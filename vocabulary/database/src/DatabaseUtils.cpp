#include <filesystem>
#include <stdexcept>

#include "DatabaseUtils.h"

std::size_t DatabaseUtils::s_referenceCount{0};
std::ofstream DatabaseUtils::s_log;

DatabaseUtils::DatabaseUtils(){
    if (!s_referenceCount){
        s_log.open("log.txt");

        if (!s_log.is_open())
            throw std::runtime_error("Logging cannot be opened");
    }
    s_referenceCount++;
}

void DatabaseUtils::checkSQLError(){
    if (m_dbCode != SQLITE_OK && m_dbCode != SQLITE_CONSTRAINT){
        s_log << "SQL error: " <<  m_errMsg << "\n";
        sqlite3_free(m_errMsg);
    }
}

DatabaseUtils::~DatabaseUtils(){
    if (m_dbCode != SQLITE_OK && m_dbCode != SQLITE_CONSTRAINT){
        s_log << "DB Error: " << sqlite3_errmsg(m_dbHandle) << "\n";
        sqlite3_close(m_dbHandle);
    }

    s_referenceCount--;
    if (!s_referenceCount && s_log.is_open())
        s_log.close();
}

bool DatabaseUtils::s_checkDatabaseExistence(std::string dbName){
    dbName = "Databases/" + dbName + ".db";
    return std::filesystem::exists(dbName);
}
