#pragma once
#include "settings.h"

#include <common/socket.h>

#include <boost/signals2.hpp>
#include <memory>
#include <QObject>
#include <QTcpServer>

class Server : public QObject
{
    Q_OBJECT
public:
    Server(const Settings::Server& settings);
    ~Server();

    boost::signals2::signal<void(std::shared_ptr<Socket>)> clientConnected;

private:
    QTcpServer m_server;

private slots:
    void OnNewConnection();
};
