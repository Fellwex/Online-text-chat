#pragma once

#include "login_info.h"
#include "registration_info.h"
#include "common/network/authorization_request.h"
#include <common/network/connection_response.h>
#include <common/network/connection_request.h>
#include <common/network/command_request.h>
#include <common/network/disconnection_request_handler.h>
#include "common/network/request_manager.h"

namespace bs = boost::signals2;

class ConnectionController
{
public:
    enum class State
    {
        Disconnected,
        EstablishingConnection,
        ConnectingServer,
        LogInOrRegistering,
        Connected,
        Disconnecting
    };

public:
    ConnectionController(RequestManager& requestManager, Connection::Ptr connection);
    ~ConnectionController();
    void Disconnect();
    State GetState() const;


    void SignIn(const std::string& host, const uint16_t port, const LoginInfo& loginInfo);
    void SignUp(const std::string& host, const uint16_t port, const RegistrationInfo& regInfo);

    bs::signal<void(const std::string&)> ErrorOccured;
    bs::signal<void(const State)> StateChanged;
    bs::signal<void(ID)> IdReceived;

private:
    void BeginConnect(const std::string& host, const uint16_t port);
    void ChangeState(const State newState);
    void SendConnectionRequest();
    Hash StringToHash(const std::string& str);
    void SendRegistrationRequest(const RegistrationInfo& registrationInfo);
    void SendLoginRequest(const LoginInfo& loginInfo);

    void OnConnectionError(Connection&, std::exception_ptr error);
    void OnConnectionEstablished(Connection&);
    void OnConnectedToServer(Request& request, Response::Ptr response);
    void OnDisconnected(Connection&);
    void OnFailedRequest(Request& request);
    void OnRegistrationOrLoginRequestFinished(Request& request, Response::Ptr response);

    State m_state { State::Disconnected };

    RequestManager& m_requestManager;
    Connection::Ptr m_connection;
    std::variant<LoginInfo, RegistrationInfo> m_contextInfo;
};
