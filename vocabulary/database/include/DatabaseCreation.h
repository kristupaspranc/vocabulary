#pragma once

#include "DatabaseUtils.h"


class DatabaseCreation: private DatabaseUtils{
    void execStatement(std::string &cmd);

public:
    DatabaseCreation(std::string & dbName);
    DatabaseCreation(const DatabaseCreation& db) = delete;
    DatabaseCreation& operator= (const DatabaseCreation& db) = delete;

    void checkDatabaseDirectory();

    void createDatabase(std::string dbName);

    void createWordTable();

    void createSentencesTable();

    void createDefinitionsTable();

    static int s_getNumberOfDatabases();
};
