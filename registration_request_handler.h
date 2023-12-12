#pragma once

#include "common/network/request_handler.h"
#include "common/network/generic_response.h"
#include "database.h"

class RegistrationRequestHandler : public RequestHandler
{
public:
    RegistrationRequestHandler() = default;
    RegistrationRequestHandler(Database::Ptr database) : m_database(database) {}
    virtual Protocol::MessageType GetType() const override;

    bs::signal<void(Connection&, const std::string&)> Registered;

private:
    virtual Response::Ptr HandleRequest(Connection& connection, Request::Ptr request) const override;

    Database::Ptr m_database;
};
