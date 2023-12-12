#include "registration_request_handler.h"

#include <common/credential_validator.h>
#include <common/network/authorization_request.h>

Protocol::MessageType RegistrationRequestHandler::GetType() const
{
    return Protocol::Registration;
}

Response::Ptr RegistrationRequestHandler::HandleRequest(Connection& connection, Request::Ptr request) const
{
    auto registrationRequest = std::dynamic_pointer_cast<AuthorizationRequest<Protocol::Registration>>(request);
    assert(registrationRequest);

    std::string errorString;

    if(!CredentialValidator::CheckCredentials(registrationRequest->GetLogin(),
                                              registrationRequest->GetPassword(),
                                              errorString))
        return std::make_shared<GenericResponse<Protocol::Registration>>(false, errorString);

    if(!m_database->FindUser(registrationRequest->GetLogin()).empty())
        return std::make_shared<GenericResponse<Protocol::Registration>>(false,
                                                                         "The user already exists");
    try
    {
        m_database->AddUser(registrationRequest->GetLogin(),
                            registrationRequest->GetPassword());

        Registered(connection, registrationRequest->GetLogin());

        return std::make_shared<GenericResponse<Protocol::Registration>>(true, "");
    }
    catch (const std::runtime_error& error)
    {
        return std::make_shared<GenericResponse<Protocol::Registration>>(false, "Database error");
    }
}
