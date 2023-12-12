#include "authorization_request_handler.h"

#include <common/credential_validator.h>
#include <common/network/generic_response.h>
#include <common/network/authorization_request.h>

AuthorizationRequestHandler::AuthorizationRequestHandler(Database::Ptr database,
                            std::function<bool(const std::string&)> checkUserActive)
    : m_database(database)
    , m_checkUserActive(checkUserActive)
{
}

Protocol::MessageType AuthorizationRequestHandler::GetType() const
{
    return Protocol::Login;
}

Response::Ptr AuthorizationRequestHandler::HandleRequest(Connection& connection, Request::Ptr request) const
{
    auto authorizationRequest = std::dynamic_pointer_cast<AuthorizationRequest<Protocol::Login>>(request);
    assert(authorizationRequest);

    std::string errorString;

    if(!CredentialValidator::CheckCredentials(authorizationRequest->GetLogin(),
                                              authorizationRequest->GetPassword(),
                                              errorString))
        return std::make_shared<GenericResponse<Protocol::Login>>(false, errorString);

    if(m_checkUserActive(authorizationRequest->GetLogin()))
        return std::make_shared<GenericResponse<Protocol::Login>>(false, "The user is already in the chat");

    try
    {
        Hash password = m_database->FindUser(authorizationRequest->GetLogin());

        if(password.empty() || password != authorizationRequest->GetPassword())
            return std::make_shared<GenericResponse<Protocol::Login>>(false, "User does not exist or password is incorrect");

        Authorized(connection, authorizationRequest->GetLogin());

        return std::make_shared<GenericResponse<Protocol::Login>>(true, "");
    }
    catch (const std::runtime_error& error)
    {
        return std::make_shared<GenericResponse<Protocol::Login>>(false, "Database error");
    }
}
