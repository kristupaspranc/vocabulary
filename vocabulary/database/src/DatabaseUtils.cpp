#include <stdexcept>

#include "DatabaseUtils.h"

std::size_t DatabaseUtils::s_referenceCount{0};
std::ofstream DatabaseUtils::s_log;

DatabaseUtils::DatabaseUtils():
    m_dbCode(SQLITE_OK)
{
    if (!s_referenceCount){
        s_log.open("log.txt");

        if (!s_log.is_open())
            throw std::runtime_error("Logging cannot be opened");
    }
    s_referenceCount++;
}

void DatabaseUtils::checkSQLError(){
    if (m_dbCode != SQLITE_OK){
        s_log << "SQL error: " <<  m_errMsg << "\n";
        sqlite3_free(m_errMsg);
    }
}

DatabaseUtils::~DatabaseUtils(){
    if (m_dbCode != SQLITE_OK){
        s_log << "DB Error: " << sqlite3_errmsg(m_dbHandle) << "\n";
    }

    if (m_dbHandle)
        sqlite3_close(m_dbHandle);

    s_referenceCount--;
    if (!s_referenceCount && s_log.is_open())
        s_log.close();
}

DatabaseUtils::DatabaseUtils(DatabaseUtils&& db){
    m_dbHandle = db.m_dbHandle;
    db.m_dbHandle = nullptr;

    m_errMsg = db.m_errMsg;
    sqlite3_free(db.m_errMsg);
}

DatabaseUtils& DatabaseUtils::operator= (DatabaseUtils&& db){
    m_dbHandle = db.m_dbHandle;
    db.m_dbHandle = nullptr;

    m_errMsg = db.m_errMsg;
    sqlite3_free(db.m_errMsg);

    return *this;
}
