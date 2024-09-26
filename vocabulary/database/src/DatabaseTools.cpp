#include "DatabaseTools.h"


DatabaseTools::DatabaseTools(std::string & dbName){
    openDatabase(dbName);
}

void DatabaseTools::openDatabase(std::string dbName){
    dbName = "Databases/" + dbName + ".db";
    dbCode = sqlite3_open_v2(dbName.c_str(), &dbHandle,
            SQLITE_OPEN_READWRITE, 0);
    checkError();
}

int DatabaseTools::callbackWord(void *data, int argc, char **argv, char **colName){
    wordsTableRow *queryResults = static_cast<wordsTableRow*>(data);

    for (std::size_t i = 0; i < argc; i++){
        (*queryResults)[i] = argv[i];
    }

    return 0;
}

void DatabaseTools::addWord(std::string & word){
    std::string cmd = R"(INSERT INTO words (word)
        VALUES (')" + word + "');";
    dbCode = sqlite3_exec(dbHandle, cmd.c_str(), NULL, NULL, &errMsg);
    checkError();
}

std::unique_ptr<wordsTableRow> DatabaseTools::lookUpWord(std::string phrase){
    std::unique_ptr<wordsTableRow> queryResults = std::make_unique<wordsTableRow>();
    wordsTableRow *ptr = queryResults.get();

    std::string cmd = R"(SELECT * FROM words WHERE word = ')" + phrase + "';";
    sqlite3_exec(dbHandle, cmd.c_str(), callbackWord, ptr, &errMsg);
    checkError();

    if ((*queryResults)[0] == "")
        return nullptr;

    return std::move(queryResults);
}

