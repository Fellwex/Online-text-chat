#pragma once

#include "connection_controller.h"

#include <common/connection.h>
#include <common/network/request_manager.h>
#include <common/unique_id_generator.h>
#include <common/user_message.h>

#include <common/protocol/protocol_structures.pb.h>

namespace bs = boost::signals2;

class ChatController
{
public:
    using Event = Protocol::ServiceMessagePayload::Event;

public:
    ChatController();
    ~ChatController() = default;
    void SendMessage(const std::string& messageText);
    ConnectionController& GetConnectionController();

    bs::signal<void(const UserMessage&)> MessageReceived;
    bs::signal<void(const std::string&, Event)> ServiceMessageReceived;
    bs::signal<void(const UserList&)> ActiveUserListUpdated;
    bs::signal<void(const std::string&)> ErrorOccured;

private:
    void OnFailedRequest(Request& request);
    void OnFinishedRequest(Request& request, Response::Ptr response);
    void OnMessageReceived(Connection&, const UserMessage& message);
    void OnServiceMessageReceived(const std::string& message, Event event);
    void OnActiveUserListUpdated(const UserList& activeUsers);

    Connection::Ptr  m_connection;

    UniqueIdGenerator    m_uniqueIdGenerator;
    RequestManager       m_requestManager;
    ConnectionController m_connectionController;
};
