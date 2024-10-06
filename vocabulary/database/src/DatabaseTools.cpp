#include "DatabaseTools.h"
#include "DatabaseUtils.h"
#include <optional>
#include <utility>


DatabaseTools::DatabaseTools(std::string & dbName){
    openDatabase(dbName);
}

void DatabaseTools::openDatabase(std::string dbName){
    dbName = "Databases/" + dbName + ".db";
    m_dbCode = sqlite3_open_v2(dbName.c_str(), &m_dbHandle,
            SQLITE_OPEN_READWRITE, 0);
    checkSQLError();
}

template<typename... Strings>
void DatabaseTools::execInsertUpdateStatement(
        std::string &cmd, allStringsInVariadic auto const&... insertives){
    sqlite3_stmt *stmt = bindText(cmd, insertives...);

    m_dbCode = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    checkSQLError();
}

bool DatabaseTools::addWord(std::string & word){
    std::string cmd = R"(INSERT INTO words (word) VALUES (?);)";
    execInsertUpdateStatement(cmd, word);

    if (m_dbCode == SQLITE_CONSTRAINT)
        return false;

    return true;
}

std::optional<wordsTableRow> DatabaseTools::lookUpWord(std::string &word){
    wordsTableRow queryResults;

    std::string cmd = R"(SELECT no_lookups, repeat_flag
        FROM words
        WHERE word = (?);)";

    sqlite3_stmt *stmt = bindText(cmd, word);

    if ((m_dbCode = sqlite3_step(stmt)) == SQLITE_ROW){
        queryResults = std::make_pair<int, bool>(
                sqlite3_column_int(stmt, 0),
                sqlite3_column_int(stmt, 1)
                );
        sqlite3_finalize(stmt);
        checkSQLError();

        return std::move(queryResults);
    }

    sqlite3_finalize(stmt);
    checkSQLError();

    return std::nullopt;
}

void DatabaseTools::addDefinition(std::string &word, std::string &definition){
    std::string cmd = R"(INSERT INTO definitions (word, definition)
        VALUES (?,?);)";
    execInsertUpdateStatement(cmd, word, definition);
}

void DatabaseTools::addSentence(std::string &word, std::string &sentence){
    std::string cmd = R"(INSERT INTO sentences (word, sentence)
        VALUES (?,?);)";
    execInsertUpdateStatement(cmd, word, sentence);
}

std::optional<std::vector<std::string>> DatabaseTools::lookUp(
        std::string &cmd, std::string & word){
    std::vector<std::string> queryResults;
    queryResults.reserve(3);

    sqlite3_stmt *stmt = bindText(cmd, word);

    while ((m_dbCode = sqlite3_step(stmt)) == SQLITE_ROW){
        queryResults.emplace_back(
                reinterpret_cast<const char*>(sqlite3_column_text(stmt,0))
                );
    }

    sqlite3_finalize(stmt);
    checkSQLError();

    if (queryResults.empty())
        return std::nullopt;

    return queryResults;
}

std::optional<std::vector<std::string>> DatabaseTools::lookUpDefinitions(std::string & word){
    std::string cmd = R"(SELECT definition
        FROM definitions
        WHERE word = (?);)";

    return lookUp(cmd, word);
}

std::optional<std::vector<std::string>> DatabaseTools::lookUpSentences(std::string & word){
    std::string cmd = R"(SELECT sentence 
        FROM sentences
        WHERE word = (?);)";

    return lookUp(cmd, word);
}

template<typename... Strings>
sqlite3_stmt* DatabaseTools::bindText(
        std::string &cmd, allStringsInVariadic auto const&... insertives){
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(m_dbHandle, cmd.c_str(), -1, &stmt, NULL);
    int i = 1;
    m_dbCode = (sqlite3_bind_text(
            stmt, i++, insertives.c_str(), -1, SQLITE_STATIC), ...);
    checkSQLError();

    return stmt;
}

std::optional<std::string> DatabaseTools::getRandomFlaggedWord(){
    std::string cmd = R"(SELECT word
        FROM words
        WHERE repeat_flag = 1
        ORDER BY RANDOM()
        LIMIT 1;)";

    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(m_dbHandle, cmd.c_str(), -1, &stmt, NULL);

    if ((m_dbCode = sqlite3_step(stmt)) == SQLITE_ROW){
        std::string random_word = 
                reinterpret_cast<const char*>(sqlite3_column_text(stmt,0));
        sqlite3_finalize(stmt);
        checkSQLError();

        return random_word;
    }

    sqlite3_finalize(stmt);
    checkSQLError();

    return std::nullopt;
}

bool DatabaseTools::singleFlaggedWordExistance(){
    std::string cmd = R"(SELECT word
        FROM words
        LIMIT 1;)";

    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(m_dbHandle, cmd.c_str(), -1, &stmt, NULL);

    if ((m_dbCode = sqlite3_step(stmt)) == SQLITE_ROW){
        sqlite3_finalize(stmt);
        checkSQLError();

        return true;
    }

    return false;
}

void DatabaseTools::unflagWord(std::string &word){
    std::string cmd = R"(UPDATE words
        SET repeat_flag = 0
        WHERE word = (?)
        )";

    execInsertUpdateStatement(cmd, word);
}




