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
    void execStatement(
            const std::string &cmd, allStringsInVariadic auto const&... insertives);

    template<typename... Strings>
    sqlite3_stmt *bindText(
            const std::string &cmd, allStringsInVariadic auto const&... insertives);

    std::optional<std::vector<std::string>> lookUp(
            const std::string &cmd, const std::string &word);

public:
    DatabaseTools() = default;
    DatabaseTools(const std::string & dbName);
    DatabaseTools(const DatabaseTools& db) = delete;
    DatabaseTools& operator= (const DatabaseTools& db) = delete;

    DatabaseTools(DatabaseTools&& db):
        DatabaseUtils(std::move(db)){}

    DatabaseTools& operator= (DatabaseTools&& db){
        if (this != &db)
            DatabaseUtils::operator=(std::move(db));
        return *this;
    }

    ~DatabaseTools() = default;


    void addWord(const std::string & word);
    std::optional<wordsTableRow> lookUpWord(const std::string &word);

    void addDefinition(const std::string &word, const std::string &definition);
    std::optional<std::vector<std::string>> lookUpDefinitions(const std::string &word);

    void addSentence(const std::string &word, const std::string &sentence);
    std::optional<std::vector<std::string>> lookUpSentences(const std::string &word);

    std::optional<std::string> getRandomFlaggedWord();

    bool singleFlaggedWordExistance();

    bool checkWordExistance(const std::string &word);

    void unflagWord(const std::string &word);

    void flagWord(const std::string &word);

    void deleteDefinition(const std::string &definition);

    void deleteSentence(const std::string &sentence);

    void incrementNumberOfLookups(const std::string &word);
};
