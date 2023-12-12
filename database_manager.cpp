#include "database_manager.h"

#include <common/logger.h>
#include <common/utils/os_utils.h>
#include <common/utils/error_handling_utils.h>

#include <sstream>
#include <QSqlError>

DatabaseManager::DatabaseManager()
    : m_database(QSqlDatabase::addDatabase("QSQLITE"))
{
}

void DatabaseManager::OpenOrCreateDatabase(const Settings::Database& settings)
{
    const std::filesystem::path absolutePathToExeFolder =
            utils::CurrentExecutableFilePath().parent_path();
    const std::string databasePath =
            (absolutePathToExeFolder / settings.m_pathToDatabase).lexically_normal().string();

    m_database.setDatabaseName(databasePath.c_str());

    if(std::filesystem::exists(databasePath))
        OpenDatabase();
    else
        CreateDatabase();
}

QSqlQuery DatabaseManager::ExecuteQueries(const std::string& stringQueries) const
{
    std::istringstream streamQueries(stringQueries);
    std::string queryString;

    QSqlQuery query(m_database);

    while(std::getline(streamQueries, queryString, ';'))
    {
        queryString += ';';

        if(!query.exec(queryString.c_str()))
            LogAndThrow<std::runtime_error>("Error executing DB query. Query: " + stringQueries
                                            + " Error: " + query.lastError().text().toStdString());

    }

    return query;
}

QSqlQuery DatabaseManager::ExecuteQuery(const std::string& stringQuery, const std::vector<QVariant>& parametersQuery) const
{
    QSqlQuery query(m_database);
    query.prepare(stringQuery.c_str());

    for(const auto& parameter : parametersQuery)
        query.addBindValue(parameter);

    if(!query.exec())
        LogAndThrow<std::runtime_error>("Error executing DB query. Query: " + stringQuery
                                        + " Error: " + query.lastError().text().toStdString());

    return query;
}

void DatabaseManager::OpenDatabase()
{
    if(!m_database.open())
        LogAndThrow<std::runtime_error>("Couldn't open database file");

    spdlog::info("Database file was opened successfully");

    if(!IsTableExists("MetaData"))
        LogAndThrow<std::runtime_error>("Database is corrupted: Table MetaData doesn't exist");

    DatabaseVersion currentVersion = GetDatabaseVersion();
    spdlog::info("Current database version - {}, Expected - {}",
                 currentVersion, headDatabaseVersion);

    if(currentVersion > headDatabaseVersion)
        LogAndThrow<std::runtime_error>("Database: Unsupported database version. Actual - "
                                        + std::to_string(currentVersion) + " Needed - "
                                        + std::to_string(headDatabaseVersion));

    while(currentVersion < headDatabaseVersion)
    {
        const std::string updgradeScript = GetUpgradeScript(currentVersion);
        ExecuteQuery(updgradeScript);
        SetDatabaseVersion(++currentVersion);
        spdlog::info("Database was upgraded from {} to {} version", currentVersion - 1, currentVersion);
    }
}

void DatabaseManager::CreateDatabase()
{
    if(!m_database.open())
        LogAndThrow<std::runtime_error>("Couln't create database file");

    ExecuteQueries(createDatabaseScript);
    SetDatabaseVersion(headDatabaseVersion);
    spdlog::info("Database file was created successfully");
    spdlog::info("Head database version - {}", headDatabaseVersion);
}

DatabaseVersion DatabaseManager::GetDatabaseVersion() const
{
    auto queryDatabaseVersion = ExecuteQuery("SELECT Value FROM MetaData WHERE Key = 'DatabaseVersion';");
    queryDatabaseVersion.next();
    return queryDatabaseVersion.value(0).toUInt();
}

void DatabaseManager::SetDatabaseVersion(const DatabaseVersion dbVersion) const
{
    ExecuteQuery("UPDATE MetaData SET Value = '" + std::to_string(dbVersion) +
                 "' WHERE Key = 'DatabaseVersion';");
}

bool DatabaseManager::IsTableExists(const std::string& tableName) const
{
    auto queryTableExists = ExecuteQuery("SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name='"
                                         + tableName + "';");
    queryTableExists.next();
    return queryTableExists.value(0).toBool();
}

std::string DatabaseManager::GetUpgradeScript(const DatabaseVersion dbVersion) const
{
    if(dbVersion < 1 || dbVersion > upgradeScripts.size())
        throw std::runtime_error("Database: Incorrect index for choice upgrade script");

    return upgradeScripts[dbVersion - 1];
}
