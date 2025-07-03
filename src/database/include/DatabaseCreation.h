#pragma once

#include "DatabaseUtils.h"

class DatabaseCreation: private DatabaseUtils{
private:
    void execStatement(std::string &cmd);

    void checkDatabaseDirectory();

    void createDatabase(std::string dbName);

    void createWordTable();

    void createSentencesTable();

    void createDefinitionsTable();

public:
    DatabaseCreation();
    DatabaseCreation(const DatabaseCreation& db) = delete;
    DatabaseCreation& operator= (const DatabaseCreation& db) = delete;
    DatabaseCreation(const DatabaseCreation&& db){}
    DatabaseCreation& operator= (const DatabaseCreation&& db){return *this;}
    ~DatabaseCreation() = default;

    void createDatabaseWithTables(const std::string &dbName);

    static int s_getNumberOfDatabases();

    static void s_deleteDatabase(std::string database);

    static bool s_checkDatabaseExistence(std::string dbName);

    static void s_renameDatabase(std::string oldName, std::string newName);
};
