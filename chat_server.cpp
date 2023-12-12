#include "chat_server.h"
#include "registration_request_handler.h"
#include "authorization_request_handler.h"

#include <common/logger.h>
#include <common/network/connection_request_handler.h>
#include <common/network/disconnection_request_handler.h>
#include <common/network/generic_response.h>
#include <common/network/message_request.h>
#include <common/network/message_request_handler.h>
#include <common/network/serializer.h>
#include <common/network/service_message_request.h>
#include <common/network/user_list_request.h>

ChatServer::ChatServer(Server& server)
    : m_server(server)
    , m_requestManager(m_uniqueIdGenerator, CreateSerializer())
{
    m_server.clientConnected.connect(
        std::bind(&ChatServer::OnUserConnected, this, std::placeholders::_1));
}

void ChatServer::InitializeDatabase(const Settings::Database& settings)
{
    auto manager = std::make_shared<DatabaseManager>();
    manager->OpenOrCreateDatabase(settings);
    m_database = std::make_unique<Database>(manager);
}

void ChatServer::OnUserConnected(std::shared_ptr<Socket> socket)
{
    auto connection = std::make_shared<Connection>(socket);

    connection->connectionClosed.connect(
        std::bind(&ChatServer::OnUserDisconnected, this, std::placeholders::_1));

    connection->Received.connect(
        std::bind(&RequestManager::HandleMessage, &m_requestManager, std::placeholders::_1,
                                                                     std::placeholders::_2));

    connection->connectionError.connect(
         std::bind(&ChatServer::OnConnectionError, this, std::placeholders::_1,
                                                         std::placeholders::_2));
    m_users.push_back({ connection });

    spdlog::info("Client connected");
}

void ChatServer::OnUserDisconnected(Connection& inactiveConnection)
{
    ActiveUserIterator user  = FindUser(inactiveConnection);

    if(user->authorized)
    {
        BroadcastRequest(std::make_shared<ServiceMessageRequest>(
                    user->nickname + " has left the chat",
                    Protocol::ServiceMessagePayload_Event_UserDisconnected),
                    inactiveConnection);

        BroadcastRequest(std::make_shared<UserListRequest>(user->nickname, false),
                        inactiveConnection);
    }

    RemoveUser(inactiveConnection);
}

void ChatServer::OnMessageReceived(Connection& connection, const UserMessage& message)
{
    UserMessage toSend = { FindUser(connection)->nickname, message.messageText };

    BroadcastRequest(std::make_shared<MessageRequest>(toSend));
}

void ChatServer::OnConnectionError(Connection& conn, const std::exception_ptr& error)
{
    if(conn.IsConnected())
    {
        conn.Disconnect();
    }
    else
    {
        RemoveUser(conn);
    }

    try
    {
        std::rethrow_exception(error);
    }
    catch (const std::runtime_error& err)
    {
        spdlog::error("Connection failed: {}", err.what());
    }
}

void ChatServer::RegisterRequestHandlers()
{
    auto connectionRequestHandler = std::make_shared<ConnectionRequestHandler>(CurrentProtocolVersion,
                                                                               m_uniqueIdGenerator);
    m_requestManager.RegisterRequestHandler(connectionRequestHandler);

    auto messageRequestHandler = std::make_shared<MessageRequestHandler>();
    messageRequestHandler->MessageReceived.connect(
                std::bind(&ChatServer::OnMessageReceived, this, std::placeholders::_1
                                                              , std::placeholders::_2));
    m_requestManager.RegisterRequestHandler(messageRequestHandler);
    m_requestManager.RegisterRequestHandler(std::make_shared<DisconnectionRequestHandler>());

    auto registrationRequestHandler = std::make_shared<RegistrationRequestHandler>(m_database);
    m_requestManager.RegisterRequestHandler(registrationRequestHandler);
    registrationRequestHandler->Registered.connect(
                std::bind(&ChatServer::OnUserAuthorized, this, std::placeholders::_1
                                                               , std::placeholders::_2));

    auto authorizationRequestHandler = std::make_shared<AuthorizationRequestHandler>(m_database,
                                          std::bind(&ChatServer::IsUserActive, this, std::placeholders::_1));
    m_requestManager.RegisterRequestHandler(authorizationRequestHandler);
    authorizationRequestHandler->Authorized.connect(
                std::bind(&ChatServer::OnUserAuthorized, this, std::placeholders::_1
                                                               , std::placeholders::_2));
}

ChatServer::ActiveUserIterator ChatServer::FindUser(Connection &connection)
{
    auto it = std::find_if(m_users.begin(), m_users.end(),
                                   [&connection](ActiveUser u)
    {
        return u.connection.get() == &connection;
    });

    assert(it != m_users.end() && "The user with such connection is not found");

    return it;
}

void ChatServer::OnUserAuthorized(Connection& connection, const std::string nickname)
{
    ActiveUserIterator user  = FindUser(connection);
    user->nickname = nickname;
    user->authorized = true;

    auto userList = std::make_shared<UserListRequest>();

    for(const auto& user : m_users)
        userList->AddUser(user.nickname, true);

    m_requestManager.SendRequest(userList, user->connection);

    BroadcastRequest(std::make_shared<ServiceMessageRequest>(nickname + " has joined the chat",
                Protocol::ServiceMessagePayload_Event_UserConnected),
                connection);
    BroadcastRequest(std::make_shared<UserListRequest>(nickname, true),
                     connection);
}

void ChatServer::RemoveUser(Connection& connection)
{
    ActiveUserIterator user = FindUser(connection);
    spdlog::debug("User <{}> removed", user->nickname);
    m_users.erase(user);
}

void ChatServer::BroadcastRequest(Request::Ptr request, boost::optional<Connection&> toSkip)
{
    for(const auto& user : m_users)
    {
        if (toSkip.has_value() && user.connection.get() == &toSkip.value())
            continue;

        try
        {
            m_requestManager.SendRequest(request->Clone(), user.connection);
        }
        catch (const std::runtime_error &error)
        {
            spdlog::error("Message sending error on connection {}: {}", user.connection->ToString(),
                                                                        error.what());
        }
    }
}

bool ChatServer::IsUserActive(const std::string& nickname)
{
    ActiveUserIterator user = std::find_if(m_users.begin(), m_users.end(),
                                           [&nickname](ActiveUser u)
    {
            return u.nickname == nickname;
    });

    return user != m_users.end();
}
