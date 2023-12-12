#pragma once
#include "database_queries.h"
#include "settings.h"

#include <QSqlDatabase>
#include <QSqlQuery>

class DatabaseManager
{
public:
    DatabaseManager();
    DatabaseManager(const DatabaseManager& other) = delete;
    void OpenOrCreateDatabase(const Settings::Database& settings);
    QSqlQuery ExecuteQueries(const std::string& stringQueries) const;
    QSqlQuery ExecuteQuery(const std::string& stringQuery,
                           const std::vector<QVariant>& parametersQuery = {}) const;

private:
    void OpenDatabase();
    void CreateDatabase();
    DatabaseVersion GetDatabaseVersion() const;
    void SetDatabaseVersion(const DatabaseVersion dbVersion) const;

    bool IsTableExists(const std::string& tableName) const;
    std::string GetUpgradeScript(const DatabaseVersion dbVersion) const;

private:
    QSqlDatabase m_database;
};
