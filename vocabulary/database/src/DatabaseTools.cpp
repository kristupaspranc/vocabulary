#include "DatabaseTools.h"
#include "DatabaseUtils.h"


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

    (*queryResults)[0] = argv[0];
    (*queryResults)[1] = argv[1];

    return 0;
}

int DatabaseTools::callbackDefinitions(void *data, int argc, char **argv, char **colName){
    definitions *queryResults = static_cast<definitions*>(data);

    for (std::size_t i = 0; i < argc; i++)
        queryResults->push_back(argv[i]);

    return 0;
}

void DatabaseTools::addWord(std::string & word){
    std::string cmd = R"(INSERT INTO words (word) VALUES (?))";
    dbCode = sqlite3_exec(dbHandle, cmd.c_str(), NULL, NULL, &errMsg);
    checkError();
}

std::unique_ptr<wordsTableRow> DatabaseTools::lookUpWord(std::string &word){
    std::unique_ptr<wordsTableRow> queryResults = std::make_unique<wordsTableRow>();
    wordsTableRow *ptr = queryResults.get();

    std::string cmd = R"(SELECT no_lookups, repeat_flag
        FROM words
        WHERE word = ')" + word + "';";
    sqlite3_exec(dbHandle, cmd.c_str(), callbackWord, ptr, &errMsg);
    checkError();

    if ((*queryResults)[0] == "")
        return nullptr;

    return std::move(queryResults);
}

void DatabaseTools::addDefinition(std::string &word, std::string &definition){
    std::string cmd = R"(INSERT INTO definitions (word, definition)
        VALUES (')" + word + "','" + definition + "');";
    dbCode = sqlite3_exec(dbHandle, cmd.c_str(), NULL, NULL, &errMsg);
    checkError();
}

std::unique_ptr<definitions> DatabaseTools::lookUpDefinitions(std::string & word){
    std::unique_ptr<definitions>
        queryResults = std::make_unique<std::vector<std::string>>();
    definitions *ptr = queryResults.get();

    std::string cmd = R"(SELECT definition
        FROM definitions
        WHERE word = ')" + word + "';";

    dbCode = sqlite3_exec(dbHandle, cmd.c_str(), callbackDefinitions, ptr, &errMsg);
    checkError();

    if (queryResults->empty())
        return nullptr;

    return std::move(queryResults);
}
