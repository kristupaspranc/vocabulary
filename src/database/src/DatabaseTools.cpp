#include "DatabaseTools.h"
#include "DatabaseUtils.h"

#include <optional>
#include <sqlite3.h>
#include <utility>


DatabaseTools::DatabaseTools(const std::string & dbName){
    openDatabase(dbName);
}

void DatabaseTools::openDatabase(std::string dbName){
    dbName = "Databases/" + dbName + ".db";
    m_dbCode = sqlite3_open_v2(dbName.c_str(), &m_dbHandle,
            SQLITE_OPEN_READWRITE, 0);
    checkSQLError();
}

template<typename... Strings>
void DatabaseTools::execStatement(
        const std::string &cmd, allStringsInVariadic auto const&... insertives){
    sqlite3_stmt *stmt = bindText(cmd, insertives...);

    m_dbCode = sqlite3_step(stmt);

    if (m_dbCode != SQLITE_DONE)
        checkSQLError();

    m_dbCode = sqlite3_finalize(stmt);
    checkSQLError();
}

void DatabaseTools::addWord(const std::string & word){
    const std::string cmd = R"(INSERT INTO words (word) VALUES (?);)";
    execStatement(cmd, word);
}

std::optional<wordsTableRow> DatabaseTools::lookUpWord(const std::string &word){
    wordsTableRow queryResults;

    const std::string cmd = R"(SELECT no_lookups, repeat_flag
        FROM words
        WHERE word = (?);)";

    sqlite3_stmt *stmt = bindText(cmd, word);

    if ((m_dbCode = sqlite3_step(stmt)) == SQLITE_ROW){
        queryResults = std::make_pair<int, bool>(
                sqlite3_column_int(stmt, 0),
                sqlite3_column_int(stmt, 1)
                );
        m_dbCode = sqlite3_finalize(stmt);
        checkSQLError();

        return std::move(queryResults);
    }

    m_dbCode = sqlite3_finalize(stmt);
    checkSQLError();

    return std::nullopt;
}

void DatabaseTools::addDefinition(const std::string &word, const std::string &definition){
    const std::string cmd = R"(INSERT INTO definitions (word, definition)
        VALUES (?,?);)";
    execStatement(cmd, word, definition);
}

void DatabaseTools::addSentence(const std::string &word, const std::string &sentence){
    const std::string cmd = R"(INSERT INTO sentences (word, sentence)
        VALUES (?,?);)";
    execStatement(cmd, word, sentence);
}

std::optional<std::vector<std::string>> DatabaseTools::lookUp(
        const std::string &cmd, const std::string & word){
    std::vector<std::string> queryResults;
    queryResults.reserve(3);

    sqlite3_stmt *stmt = bindText(cmd, word);

    while ((m_dbCode = sqlite3_step(stmt)) == SQLITE_ROW){
        queryResults.emplace_back(
                reinterpret_cast<const char*>(sqlite3_column_text(stmt,0))
                );
    }

    m_dbCode = sqlite3_finalize(stmt);
    checkSQLError();

    if (queryResults.empty())
        return std::nullopt;

    return queryResults;
}

std::optional<std::vector<std::string>> DatabaseTools::lookUpDefinitions(const std::string & word){
    const std::string cmd = R"(SELECT definition
        FROM definitions
        WHERE word = (?);)";

    return lookUp(cmd, word);
}

std::optional<std::vector<std::string>> DatabaseTools::lookUpSentences(const std::string & word){
    const std::string cmd = R"(SELECT sentence 
        FROM sentences
        WHERE word = (?);)";

    return lookUp(cmd, word);
}

template<typename... Strings>
sqlite3_stmt* DatabaseTools::bindText(
        const std::string &cmd, allStringsInVariadic auto const&... insertives){
    sqlite3_stmt *stmt;

    m_dbCode = sqlite3_prepare_v2(m_dbHandle, cmd.c_str(), -1, &stmt, NULL);
    checkSQLError();

    int i = 1;
    ((m_dbCode == SQLITE_OK ? m_dbCode = sqlite3_bind_text(stmt, i++, insertives.c_str(), -1, 
                                                             SQLITE_STATIC) : m_dbCode), ...);
    checkSQLError();

    return stmt;
}

std::optional<std::string> DatabaseTools::getRandomFlaggedWord(){
    const std::string cmd = R"(SELECT word
        FROM words
        WHERE repeat_flag = 1
        ORDER BY RANDOM()
        LIMIT 1;)";

    sqlite3_stmt *stmt;
    m_dbCode = sqlite3_prepare_v2(m_dbHandle, cmd.c_str(), -1, &stmt, NULL);
    checkSQLError();

    if ((m_dbCode = sqlite3_step(stmt)) == SQLITE_ROW){
        const std::string random_word = 
                reinterpret_cast<const char*>(sqlite3_column_text(stmt,0));
        m_dbCode = sqlite3_finalize(stmt);
        checkSQLError();

        return random_word;
    }

    m_dbCode = sqlite3_finalize(stmt);
    checkSQLError();

    return std::nullopt;
}

bool DatabaseTools::singleFlaggedWordExistance(){
    const std::string cmd = R"(SELECT word
        FROM words
        LIMIT 1;)";

    sqlite3_stmt *stmt;
    m_dbCode = sqlite3_prepare_v2(m_dbHandle, cmd.c_str(), -1, &stmt, NULL);
    checkSQLError();

    if ((m_dbCode = sqlite3_step(stmt)) == SQLITE_ROW){
        m_dbCode = sqlite3_finalize(stmt);
        checkSQLError();

        return true;
    }

    return false;
}

bool DatabaseTools::checkWordExistance(const std::string &word){
    const std::string cmd = R"(SELECT word 
        FROM words
        WHERE word = (?)
        LIMIT 1;)";

    sqlite3_stmt *stmt = bindText(cmd, word);

    if ((m_dbCode = sqlite3_step(stmt)) == SQLITE_ROW){
        m_dbCode = sqlite3_finalize(stmt);
        checkSQLError();

        flagWord(word);

        return true;
    }

    return false;
}

void DatabaseTools::unflagWord(const std::string &word){
    const std::string cmd = R"(UPDATE words
        SET repeat_flag = 0
        WHERE word = (?)
        )";

    execStatement(cmd, word);
}

void DatabaseTools::flagWord(const std::string &word){
    const std::string cmd = R"(UPDATE words
        SET repeat_flag = 1
        WHERE word = (?)
        )";

    execStatement(cmd, word);
}

void DatabaseTools::deleteDefinition(const std::string &definition){
    const std::string cmd = R"(DELETE FROM definitions 
        WHERE definition = (?)
        )";

    execStatement(cmd, definition);
}

void DatabaseTools::deleteSentence(const std::string &sentence){
    const std::string cmd = R"(DELETE FROM sentences 
        WHERE sentence = (?)
        )";

    execStatement(cmd, sentence);
}

void DatabaseTools::incrementNumberOfLookups(const std::string &word){
    const std::string cmd = R"(UPDATE words
        SET no_lookups = no_lookups + 1
        WHERE word = (?);
        )";

    execStatement(cmd, word);
}





