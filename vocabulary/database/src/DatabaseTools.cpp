#include "DatabaseTools.h"


bool DatabaseTools::checkDatabaseExistence(std::string dbName){
    return std::filesystem::exists(dbName);
}

void DatabaseTools::openDatabase(std::string dbName){
    dbName = "Databases/" + dbName + ".db";
    dbCode = sqlite3_open_v2(dbName.c_str(), &dbHandle,
            SQLITE_OPEN_READWRITE, 0);
    checkError();
}

int DatabaseTools::callbackWord(void *data, int argc, char **argv, char **colName){
    wordsTable *queryResults = static_cast<wordsTable*>(data);
    wordsTableRow row;
    for (std::size_t i = 0; i < argc; i++){
        row[i] = argv[i];
    }

    queryResults->push_back(row);

    return 0;
}

DatabaseTools::DatabaseTools(std::string & dbName){
    openDatabase(dbName);
}

void DatabaseTools::addWord(std::string & word){
    std::string cmd = R"(INSERT INTO words (word) VALUES ()" + word + ");";
    dbCode = sqlite3_exec(dbHandle, cmd.c_str(), NULL, NULL, &errMsg);
    checkError();
}

std::unique_ptr<wordsTable> DatabaseTools::lookUpWord(std::string phrase){
    std::unique_ptr<wordsTable> queryResults = std::make_unique<wordsTable>();
    wordsTable *ptr = queryResults.get();

    std::string cmd = R"(SELECT * FROM words WHERE word = ')" + phrase + "';";
    sqlite3_exec(dbHandle, cmd.c_str(), callbackWord, ptr, &errMsg);

    return std::move(queryResults);
}
