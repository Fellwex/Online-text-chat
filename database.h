#pragma once
#include "database_manager.h"
#include "common/types.h"

#include <memory>

class Database
{
public:
    using Ptr = std::shared_ptr<Database>;

    Database(std::shared_ptr<DatabaseManager> databaseManager);
    Hash FindUser(const std::string& login) const;
    void AddUser(const std::string& login, const Hash& password);

private:
    std::shared_ptr<DatabaseManager> m_databaseManager;
};

