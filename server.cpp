#include "server.h"

#include <common/logger.h>
#include <common/utils/error_handling_utils.h>

Server::Server(const Settings::Server& settings)
{
    if(m_server.listen(QHostAddress::Any, settings.m_port))
    {
        spdlog::info("Server started listening. Port - {}", settings.m_port);
    }
    else
    {
        LogAndThrow<std::runtime_error>("Failed to start listening. Port - "
                                        + std::to_string(settings.m_port));
    }

    connect(&m_server, SIGNAL(newConnection()), this, SLOT(OnNewConnection()));
}

Server::~Server()
{
    m_server.close();
}

void Server::OnNewConnection()
{
    auto connection = std::make_shared<Socket>(m_server.nextPendingConnection());
    clientConnected(connection);
}
