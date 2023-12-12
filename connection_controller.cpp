#include "connection_controller.h"

#include <common/credential_validator.h>

#include <boost/functional/hash.hpp>
#include <sstream>

ConnectionController::ConnectionController(RequestManager& requestManager, Connection::Ptr connection)
    : m_requestManager(requestManager)
    , m_connection(connection)
{
    m_connection->connectionOpened.connect(
                std::bind(&ConnectionController::OnConnectionEstablished, this, std::placeholders::_1));
    m_connection->connectionClosed.connect(
                std::bind(&ConnectionController::OnDisconnected, this, std::placeholders::_1));
    m_connection->connectionError.connect(
        std::bind(&ConnectionController::OnConnectionError, this, std::placeholders::_1,
                                                                  std::placeholders::_2));
}

ConnectionController::~ConnectionController()
{
    if (m_state == State::Connected)
        Disconnect();
}

void ConnectionController::BeginConnect(const std::string& host, const uint16_t port)
{
    ExpectState<State>(m_state, State::Disconnected, __func__);
    ChangeState(State::EstablishingConnection);
    m_connection->Connect(host, port);
}

void ConnectionController::SendRegistrationRequest(const RegistrationInfo& registrationInfo)
{
    ExpectState<State>(m_state, State::ConnectingServer, __func__);
    ChangeState(State::LogInOrRegistering);

    auto registrationRequest =
            std::make_shared<AuthorizationRequest<Protocol::Registration>>(registrationInfo.login,
                                                                           StringToHash(registrationInfo.password));
    registrationRequest->finished.connect(
                std::bind(&ConnectionController::OnRegistrationOrLoginRequestFinished, this, std::placeholders::_1,
                                                                                             std::placeholders::_2));

    registrationRequest->failed.connect(std::bind(&ConnectionController::OnFailedRequest,
                                                  this, std::placeholders::_1));

    m_requestManager.SendRequest(registrationRequest, m_connection);
}

void ConnectionController::SendLoginRequest(const LoginInfo& loginInfo)
{
    ExpectState<State>(m_state, State::ConnectingServer, __func__);
    ChangeState(State::LogInOrRegistering);

    auto loginRequest =
            std::make_shared<AuthorizationRequest<Protocol::Login>>(loginInfo.login, StringToHash(loginInfo.password));

    loginRequest->finished.connect(
                std::bind(&ConnectionController::OnRegistrationOrLoginRequestFinished, this, std::placeholders::_1,
                                                                                             std::placeholders::_2));

    loginRequest->failed.connect(std::bind(&ConnectionController::OnFailedRequest,
                                                  this, std::placeholders::_1));

    m_requestManager.SendRequest(loginRequest, m_connection);
}

void ConnectionController::SendConnectionRequest()
{
    ExpectState<State>(m_state, State::EstablishingConnection, __func__);
    ChangeState(State::ConnectingServer);

    auto connectionRequest = std::make_shared<ConnectionRequest>(CurrentProtocolVersion);
    connectionRequest->finished.connect(std::bind(&ConnectionController::OnConnectedToServer,
                                                           this, std::placeholders::_1,
                                                                 std::placeholders::_2));

    connectionRequest->failed.connect(std::bind(&ConnectionController::OnFailedRequest,
                                                     this, std::placeholders::_1));

    m_requestManager.SendRequest(connectionRequest, m_connection);
}

Hash ConnectionController::StringToHash(const std::string &str)
{
    std::stringstream ss;
    ss << std::hex << boost::hash<std::string>{}(str);
    return ss.str();
}

void ConnectionController::Disconnect()
{
    std::vector<State> expectedStates
    {
        State::ConnectingServer,
        State::LogInOrRegistering,
        State::Connected
    };

    ExpectStates<State>(m_state, expectedStates, __func__);
    ChangeState(State::Disconnecting);
    spdlog::info("Requesting disconnection");

    auto disconnectionRequest = std::make_shared<CommandRequest<Protocol::MessageType::Disconnect>>();
    m_requestManager.SendRequest(disconnectionRequest, m_connection);
}

void ConnectionController::SignIn(const std::string& host, const uint16_t port, const LoginInfo &loginInfo)
{
    ExpectState<State>(m_state, State::Disconnected, __func__);
    m_contextInfo = loginInfo;
    BeginConnect(host, port);
}

void ConnectionController::SignUp(const std::string& host, const uint16_t port, const RegistrationInfo &regInfo)
{
    ExpectState<State>(m_state, State::Disconnected, __func__);
    m_contextInfo = regInfo;
    BeginConnect(host, port);
}

void ConnectionController::ChangeState(const State newState)
{
    if(newState == m_state)
        return;

    spdlog::info("ConnectionController state is changed from:{} to:{}", static_cast<int>(m_state),
                                                      static_cast<int>(newState));
    m_state = newState;
    StateChanged(newState);
}

void ConnectionController::OnConnectionError(Connection&, std::exception_ptr error)
{
    try
    {
        std::rethrow_exception(error);
    }
    catch (const std::runtime_error& err)
    {
        spdlog::error("Connection failed: {}", err.what());
        ErrorOccured(std::string("Connection failed: ") + err.what());
    }
}

void ConnectionController::OnConnectionEstablished(Connection &)
{
    spdlog::info("Established connection with a server");
    SendConnectionRequest();
}

void ConnectionController::OnConnectedToServer(Request& request, Response::Ptr response)
{
    spdlog::info("Connected to server");
    auto connectionResponse = std::dynamic_pointer_cast<ConnectionResponse>(response);
    assert(connectionResponse);

    if(connectionResponse->GetSuccess())
    {
        IdReceived(connectionResponse->GetInstanceID());
        std::holds_alternative<LoginInfo>(m_contextInfo) ? SendLoginRequest(std::get<LoginInfo>(m_contextInfo))
                                                         : SendRegistrationRequest(std::get<RegistrationInfo>(m_contextInfo));
    }
    else
    {
        spdlog::error("Connection failed: {}", connectionResponse->GetErrorString());
        ErrorOccured(std::string("Connection failed: ") + connectionResponse->GetErrorString());
        Disconnect();
    }
}

void ConnectionController::OnDisconnected(Connection &)
{
    ChangeState(State::Disconnected);
    spdlog::info("Disconnected from server");
}

void ConnectionController::OnFailedRequest(Request &request)
{
    switch (request.GetMessageType())
    {
    case Protocol::Connect:
        spdlog::error("Couldn't sent connection request to the server");
        ErrorOccured("Connection failed: Couldn't sent connection request to the server");
        Disconnect();
        break;

    case Protocol::Registration:
        spdlog::error("Couldn't sent registration request to the server");
        ErrorOccured("Registration failed: Couldn't sent registration request to the server");
        Disconnect();
        break;

    case Protocol::Login:
        spdlog::error("Couldn't sent authorization request to the server");
        ErrorOccured("Authorization failed: Couldn't sent authorization request to the server");
        Disconnect();
        break;

    default:
        spdlog::error("Unsupported response type: {}", request.GetMessageType());
        break;
    }
}

void ConnectionController::OnRegistrationOrLoginRequestFinished(Request &request, Response::Ptr response)
{
    if(response->GetMessageType() != Protocol::MessageType::Registration &&
            response->GetMessageType() != Protocol::MessageType::Login)
        LogAndThrow<std::logic_error>("Message type can be only login or registration");

    if(response->GetMessageType() == Protocol::MessageType::Registration && !response->GetSuccess())
    {
        spdlog::info("Unsuccessful registration: {}", response->GetErrorString());
        ErrorOccured("Cannot create new user: " + response->GetErrorString());
        Disconnect();
    }
    else if(response->GetMessageType() == Protocol::MessageType::Login && !response->GetSuccess())
    {
        spdlog::info("Unsuccessful authorization: {}", response->GetErrorString());
        ErrorOccured("Couldn't sign in: " + response->GetErrorString());
        Disconnect();
    }
    else
    {
        ChangeState(State::Connected);
    }
}
