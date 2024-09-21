#pragma once

#include<array>
#include<filesystem>
#include<vector>

#include "DatabaseUtils.h"

using wordsTableRow = std::array<std::string, 3>;
using wordsTable = std::vector<wordsTableRow>;

class DatabaseTools: private DatabaseUtils{
    bool checkDatabaseExistence(std::string dbName);

    void openDatabase(std::string dbName);

    static int callbackWord(void *data, int argc, char **argv, char **colName);

public:
    DatabaseTools(std::string & dbName);
    DatabaseTools(const DatabaseTools& db) = delete;
    DatabaseTools& operator= (const DatabaseTools& db) = delete;

    void addWord(std::string & word);

    std::unique_ptr<wordsTable> lookUpWord(std::string phrase);
};
