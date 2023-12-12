#include "chat_controller.h"

#include <common/logger.h>
#include <common/utils/error_handling_utils.h>
#include <common/network/generic_response.h>
#include <common/network/message_request.h>
#include <common/network/message_request_handler.h>
#include <common/network/service_message_request_handler.h>
#include <common/network/serializer.h>
#include <common/network/user_list_request_handler.h>

ChatController::ChatController()
    : m_connection(std::make_shared<Connection>(std::make_shared<Socket>()))
    , m_requestManager(m_uniqueIdGenerator, CreateSerializer())
    , m_connectionController(m_requestManager, m_connection)
{
    m_connectionController.IdReceived.connect(
                std::bind(&UniqueIdGenerator::SetInstanceID, &m_uniqueIdGenerator, std::placeholders::_1));
    m_connection->Received.connect(
                    std::bind(&RequestManager::HandleMessage, &m_requestManager, std::placeholders::_1
                                                                               , std::placeholders::_2));

    auto messageRequestHandler = std::make_shared<MessageRequestHandler>();
    messageRequestHandler->MessageReceived.connect(
                std::bind(&ChatController::OnMessageReceived, this, std::placeholders::_1
                                                                  , std::placeholders::_2));
    m_requestManager.RegisterRequestHandler(messageRequestHandler);

    auto serviceMessageRequestHandler = std::make_shared<ServiceMessageRequestHandler>();
    serviceMessageRequestHandler->MessageReceived.connect(
                std::bind(&ChatController::OnServiceMessageReceived, this, std::placeholders::_1
                                                                         , std::placeholders::_2));
    m_requestManager.RegisterRequestHandler(serviceMessageRequestHandler);

    m_requestManager.RegisterRequestHandler(std::make_shared<DisconnectionRequestHandler>());

    auto userListRequestHandler = std::make_shared<UserListRequestHandler>();
    userListRequestHandler->UserListUpdated.connect(
                std::bind(&ChatController::OnActiveUserListUpdated, this, std::placeholders::_1));
    m_requestManager.RegisterRequestHandler(userListRequestHandler);
}

void ChatController::SendMessage(const std::string &messageText)
{
    UserMessage message { "", messageText };

    auto messageRequest = std::make_shared<MessageRequest>(message);
    messageRequest->finished.connect(
                std::bind(&ChatController::OnFinishedRequest, this, std::placeholders::_1
                                                                  , std::placeholders::_2));
    messageRequest->failed.connect(
                std::bind(&ChatController::OnFailedRequest, this, std::placeholders::_1));
    m_requestManager.SendRequest(messageRequest, m_connection);
}

ConnectionController& ChatController::GetConnectionController()
{
    return m_connectionController;
}

void ChatController::OnFinishedRequest(Request &request, Response::Ptr response)
{
    switch (request.GetMessageType())
    {
    case Protocol::Message:
        if(!response->GetSuccess())
        {
            spdlog::error("Unsuccessful request: {}", response->GetErrorString());
            ErrorOccured(response->GetErrorString());
        }
        break;

    default:
        spdlog::error("Unsupported response type");
        break;
    }
}

void ChatController::OnFailedRequest(Request& request)
{
    switch (request.GetMessageType())
    {
    case Protocol::Message:
        spdlog::error("Unsuccessful attempt to send message");
        ErrorOccured("Cannot send message, please check your connecion and authorization state");
        break;

    default:
        spdlog::error("Unsupported response type: {}", request.GetMessageType());
        break;
    }
}

void ChatController::OnMessageReceived(Connection&, const UserMessage& message)
{
    MessageReceived(message);
}

void ChatController::OnServiceMessageReceived(const std::string& message, Event event)
{
    ServiceMessageReceived(message, event);
}

void ChatController::OnActiveUserListUpdated(const UserList& activeUsers)
{
    ActiveUserListUpdated(activeUsers);
}
