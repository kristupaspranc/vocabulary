#include "DatabaseCreation.h"


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
    // TODO implement catching
    catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    }
}

void DatabaseCreation::createDatabase(std::string dbName){
    dbCode = sqlite3_open_v2(dbName.c_str(), &dbHandle,
            SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, 0);

    checkError();
}

void DatabaseCreation::createWordTable(){
    std::string cmd = R"(CREATE TABLE IF NOT EXISTS words(
            word TEXT PRIMARY KEY,
            no_lookups INTEGER DEFAULT 0,
            repeat_flag TINYINT DEFAULT 1))";

    dbCode = sqlite3_exec(dbHandle, cmd.c_str(), NULL, NULL, &errMsg);
    checkError();
}

void DatabaseCreation::createSentencesTable(){
    std::string cmd = R"(CREATE TABLE IF NOT EXISTS sentences(
            word TEXT NOT NULL,
            sentence TEXT NOT NULL,
            PRIMARY KEY (word, sentence),
            FOREIGN KEY (word) REFERENCES words(word)))";

    dbCode = sqlite3_exec(dbHandle, cmd.c_str(), NULL, NULL, &errMsg);
    checkError();
}

void DatabaseCreation::createDefinitionsTable(){
    std::string cmd = R"(CREATE TABLE IF NOT EXISTS definitions(
            word TEXT NOT NULL,
            definition TEXT NOT NULL,
            PRIMARY KEY (word, definition),
            FOREIGN KEY (word) REFERENCES words(word)))";

    dbCode = sqlite3_exec(dbHandle, cmd.c_str(), NULL, NULL, &errMsg);
    checkError();
}
