#pragma once

#include "chat_controller.h"
#include "connection_dialog.h"

#include <QMainWindow>

namespace Ui {
    class ChatWindow;
}

class ChatWindow : public QMainWindow
{
    Q_OBJECT

public:
    using Event = Protocol::ServiceMessagePayload::Event;

public:
    explicit ChatWindow(ChatController& controller, QWidget *parent = nullptr);
    void InitiateDisconnection();
    void ShowConnectionDialog();
    void ShowRegistrationDialog(const std::string& host, uint16_t port);
    ~ChatWindow();

private:
    void Send();

    void OnConnectionControlerStateChanged(ConnectionController::State state);
    void OnMessageReceived(const UserMessage& message);
    void OnServiceMessageReceived(const std::string& message, Event event);
    void OnActiveUserListUpdated(const UserList& activeUsers);
    void OnErrorMessage(const std::string& errText);

private slots:
    void on_actionConnect_triggered();
    void on_pushButton_clicked();
    void on_actionDisconnect_triggered();
    void on_actionQuit_triggered();

private:
    Ui::ChatWindow* m_ui;
    ChatController& m_controller;
    bs::connection m_connectionErrorHandling;
};
