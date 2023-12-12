#pragma once

#include "common/network/request_handler.h"
#include "database.h"

class AuthorizationRequestHandler : public RequestHandler
{
public:

    AuthorizationRequestHandler() = default;
    AuthorizationRequestHandler(Database::Ptr database,
                                std::function<bool(const std::string&)> checkUserActive);
    virtual Protocol::MessageType GetType() const override;

    bs::signal<void(Connection&, const std::string&)> Authorized;

private:
    virtual Response::Ptr HandleRequest(Connection& connection, Request::Ptr request) const override;

    Database::Ptr m_database;
    std::function<bool(const std::string&)> m_checkUserActive;
};
