#include "DatabaseUtils.h"

#include<iostream>
#include <filesystem>

class DatabaseCreation: private DatabaseUtils{
public:
    DatabaseCreation(std::string & dbName);
    DatabaseCreation(const DatabaseCreation& db) = delete;
    DatabaseCreation& operator= (const DatabaseCreation& db) = delete;

    void checkDatabaseDirectory();

    void createDatabase(std::string dbName);

    void createWordTable();

    void createSentencesTable();

    void createDefinitionsTable();
};
