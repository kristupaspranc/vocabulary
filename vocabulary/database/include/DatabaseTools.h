#pragma once

#include<array>
#include<vector>
#include<memory>

#include "DatabaseUtils.h"

using wordsTableRow = std::array<std::string, 2>;
using definitions = std::vector<std::string>;

template <typename... T>
concept allStringsInVariadic = (std::is_same_v<T, std::string> && ...);

class DatabaseTools: private DatabaseUtils{
    bool checkDatabaseExistence(std::string dbName);

    void openDatabase(std::string dbName);

    static int callbackWord(void *data, int argc, char **argv, char **colName);

    static int callbackDefinitions(void *data, int argc, char **argv, char **colName);

    template<typename... strings>
    void execInsertStatement(
            std::string &cmd, allStringsInVariadic auto const&... insertives);

public:
    DatabaseTools(std::string & dbName);
    DatabaseTools(const DatabaseTools& db) = delete;
    DatabaseTools& operator= (const DatabaseTools& db) = delete;

    void addWord(std::string & word);
    std::unique_ptr<wordsTableRow> lookUpWord(std::string &word);

    void addDefinition(std::string &word, std::string &definition);
    std::unique_ptr<definitions> lookUpDefinitions(std::string &word);
};
