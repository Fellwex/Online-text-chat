#include "database.h"

Database::Database(std::shared_ptr<DatabaseManager> databaseManager)
    : m_databaseManager(databaseManager)
{
}

Hash Database::FindUser(const std::string& login) const
{
    QSqlQuery userPassword = m_databaseManager->ExecuteQuery("SELECT Password FROM Users WHERE Login = ?;",
                                                             {login.c_str()});

    userPassword.next();
    return userPassword.first() ? userPassword.value(0).toString().toStdString() : "";
}

void Database::AddUser(const std::string& login, const Hash& password)
{
    m_databaseManager->ExecuteQuery("INSERT INTO Users(Login, Password) VALUES (?, ?);",
                                    {login.c_str(), password.c_str()});
}
