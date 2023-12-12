#pragma once
#include "active_user.h"
#include "server.h"
#include "database.h"

#include <common/connection.h>
#include <common/unique_id_generator.h>
#include <common/user_message.h>
#include <common/network/request_manager.h>

#include <list>
#include <memory>
#include <optional>

class ChatServer
{
public:
    ChatServer(Server& server);
    void InitializeDatabase(const Settings::Database& settings);
    void RegisterRequestHandlers();

private:
    using ActiveUserList = std::list<ActiveUser>;
    using ActiveUserIterator = ActiveUserList::iterator;
    using UserEvent = Protocol::ServiceMessagePayload::Event;

    ActiveUserIterator FindUser(Connection& connection);
    void RemoveUser(Connection& connection);
    void BroadcastRequest(Request::Ptr request, boost::optional<Connection&> toSkip = {});
    bool IsUserActive(const std::string& nickname);

    void OnUserAuthorized(Connection& connection, const std::string nickname);
    void OnUserConnected(std::shared_ptr<Socket> socket);
    void OnUserDisconnected(Connection& inactiveConnection);
    void OnConnectionError(Connection& connection, const std::exception_ptr& error);
    void OnMessageReceived(Connection&, const UserMessage& message);

    ActiveUserList m_users;
    Server& m_server;

    UniqueIdGenerator m_uniqueIdGenerator;
    RequestManager m_requestManager;

    Database::Ptr m_database;
};
