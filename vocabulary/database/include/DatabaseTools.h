#pragma once

#include <utility>
#include <vector>
#include <optional> 

#include "DatabaseUtils.h"

using wordsTableRow = std::pair<int, bool>;

template <typename... T>
concept allStringsInVariadic = (std::is_same_v<T, std::string> && ...);

class DatabaseTools: private DatabaseUtils{
    void openDatabase(std::string dbName);

    template<typename... Strings>
    void execInsertUpdateStatement(
            std::string &cmd, allStringsInVariadic auto const&... insertives);

    template<typename... Strings>
    sqlite3_stmt *bindText(
            std::string &cmd, allStringsInVariadic auto const&... insertives);

    std::optional<std::vector<std::string>> lookUp(
            std::string &cmd, std::string & word);

public:
    DatabaseTools(std::string & dbName);
    DatabaseTools(const DatabaseTools& db) = delete;
    DatabaseTools& operator= (const DatabaseTools& db) = delete;

    bool addWord(std::string & word);
    std::optional<wordsTableRow> lookUpWord(std::string &word);

    void addDefinition(std::string &word, std::string &definition);
    std::optional<std::vector<std::string>> lookUpDefinitions(std::string &word);

    void addSentence(std::string &word, std::string &sentence);
    std::optional<std::vector<std::string>> lookUpSentences(std::string &word);

    std::optional<std::string> getRandomFlaggedWord();

    bool singleFlaggedWordExistance();

    void unflagWord(std::string &word);

    void flagWord(std::string &word);
};
