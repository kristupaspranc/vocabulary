#include <stdexcept>

#include "DatabaseUtils.h"

std::size_t DatabaseUtils::s_referenceCount{0};
std::ofstream DatabaseUtils::s_log;

void DatabaseUtils::acquireReference() {
    if (s_referenceCount == 0) {
        s_log.open("log.txt");
        if (!s_log.is_open()) {
            throw std::runtime_error("Cannot open log.txt");
        }
    }
    s_referenceCount++;
}

void DatabaseUtils::releaseReference() {
    s_referenceCount--;
    if (s_referenceCount == 0 && s_log.is_open()) {
        s_log.close();
    }
}

DatabaseUtils::DatabaseUtils():
    m_dbCode(SQLITE_OK)
{
    acquireReference();
}

void DatabaseUtils::checkSQLError(){
    if (m_dbCode != SQLITE_OK){
        s_log << m_dbCode << " " << (m_errMsg ? m_errMsg : "(null)") << "\n";
        s_log.flush();

        if (m_errMsg){
            sqlite3_free(m_errMsg);
            m_errMsg = nullptr;
        }

        s_log << std::unitbuf;
    }
}

DatabaseUtils::~DatabaseUtils(){
    if (m_dbHandle){
        if (m_dbCode != SQLITE_OK){
            s_log << "DB Error: " << sqlite3_errmsg(m_dbHandle) << "\n";
            s_log.flush();
        }

        sqlite3_close(m_dbHandle);
        m_dbHandle = nullptr;
    }

    if (m_errMsg)
        sqlite3_free(m_errMsg);

    releaseReference();
}

DatabaseUtils::DatabaseUtils(DatabaseUtils&& db):
    m_dbHandle(db.m_dbHandle), m_errMsg(db.m_errMsg), m_dbCode(db.m_dbCode)
{
    db.m_dbHandle = nullptr;
    db.m_errMsg = nullptr;
    db.m_dbCode = SQLITE_OK;
    acquireReference();
}

DatabaseUtils& DatabaseUtils::operator= (DatabaseUtils&& db){
    if (this != &db) {
        if (m_dbHandle){
            if (m_dbCode != SQLITE_OK){
                s_log << "DB Error: " << sqlite3_errmsg(m_dbHandle) << "\n";
                s_log.flush();
            }

            sqlite3_close(m_dbHandle);
            m_dbHandle = nullptr;
        }

        if (m_errMsg) {
            sqlite3_free(m_errMsg);
        }

        m_dbHandle = db.m_dbHandle;
        m_errMsg = db.m_errMsg;
        m_dbCode = db.m_dbCode;

        db.m_dbHandle = nullptr;
        db.m_errMsg = nullptr;
        db.m_dbCode = SQLITE_OK;

        acquireReference();
    }

    return *this;
}

//void DatabaseUtils::printDatabaseFileName() {
    //sqlite3_stmt* stmt;
    //const char* sql = "PRAGMA database_list;";
    //int rc = sqlite3_prepare_v2(m_dbHandle, sql, -1, &stmt, nullptr);

    //if (rc != SQLITE_OK) {
        //s_log << rc << "Failed to prepare statement: " << sqlite3_errmsg(m_dbHandle) << "\n";
        //s_log.flush();

        //s_log << std::unitbuf;
    //}

    //while (sqlite3_step(stmt) == SQLITE_ROW) {
        //int seq = sqlite3_column_int(stmt, 0);
        //const unsigned char* name = sqlite3_column_text(stmt, 1);
        //const unsigned char* file = sqlite3_column_text(stmt, 2);

        //s_log << "Database [" << seq << "] Name: " << name 
                  //<< ", File: " << (file ? reinterpret_cast<const char*>(file) : "(in-memory)") 
                  //<< std::endl;
    //}

    //sqlite3_finalize(stmt);
//}
