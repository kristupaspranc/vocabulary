#include "DatabaseCreation.h"

#include <algorithm>
#include <filesystem>


DatabaseCreation::DatabaseCreation(std::string & dbName){
    dbName = "Databases/" + dbName + ".db";

    checkDatabaseDirectory();
    createDatabase(dbName);
    createWordTable();
    createSentencesTable();
    createDefinitionsTable();
}

void DatabaseCreation::checkDatabaseDirectory(){
    try {
        std::filesystem::create_directory(std::filesystem::current_path() / "Databases");
    }
    catch (const std::filesystem::filesystem_error& e) {
        m_log << "Filesystem error: " << e.what() << "\n";
    }
}

void DatabaseCreation::createDatabase(std::string dbName){
    m_dbCode = sqlite3_open_v2(dbName.c_str(), &m_dbHandle,
            SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, 0);

    checkSQLError();
}

void DatabaseCreation::createWordTable(){
    std::string cmd = R"(CREATE TABLE IF NOT EXISTS words(
            word TEXT PRIMARY KEY,
            no_lookups INTEGER DEFAULT 0,
            repeat_flag TINYINT DEFAULT 1))";

    execStatement(cmd);
}

void DatabaseCreation::createSentencesTable(){
    std::string cmd = R"(CREATE TABLE IF NOT EXISTS sentences(
            word TEXT NOT NULL,
            sentence TEXT NOT NULL,
            PRIMARY KEY (word, sentence),
            FOREIGN KEY (word) REFERENCES words(word)))";

    execStatement(cmd);
}

void DatabaseCreation::createDefinitionsTable(){
    std::string cmd = R"(CREATE TABLE IF NOT EXISTS definitions(
            word TEXT NOT NULL,
            definition TEXT NOT NULL,
            PRIMARY KEY (word, definition),
            FOREIGN KEY (word) REFERENCES words(word)))";

    execStatement(cmd);
}

void DatabaseCreation::execStatement(std::string &cmd){
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(m_dbHandle, cmd.c_str(), -1, &stmt, NULL);
    m_dbCode = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    checkSQLError();
}


int DatabaseCreation::s_getNumberOfDatabases(){
    std::filesystem::directory_iterator dirIter;

    try{
        dirIter = std::filesystem::directory_iterator(
                std::filesystem::current_path() / "Databases"
                );
    }
    catch (const std::filesystem::filesystem_error& e){
        return 0;
    }

    return std::count_if(
            begin(dirIter),
            end(dirIter),
            [](auto& entry) { return entry.is_regular_file(); }
            );
}
