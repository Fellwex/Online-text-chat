#include "chat_window.h"
#include "ui_chat_window.h"

#include <common/logger.h>

#include <QMessageBox>

ChatWindow::ChatWindow(ChatController& controller, QWidget *parent)
    : m_controller(controller)
    , QMainWindow(parent)
    , m_ui(new Ui::ChatWindow)
{
    m_ui->setupUi(this);

    m_controller.MessageReceived.connect(
        std::bind(&ChatWindow::OnMessageReceived, this, std::placeholders::_1));

    m_controller.ServiceMessageReceived.connect(
        std::bind(&ChatWindow::OnServiceMessageReceived, this, std::placeholders::_1
                                                             , std::placeholders::_2));

    m_controller.ActiveUserListUpdated.connect(
        std::bind(&ChatWindow::OnActiveUserListUpdated, this, std::placeholders::_1));

    m_controller.GetConnectionController().StateChanged.connect(
                std::bind(&ChatWindow::OnConnectionControlerStateChanged, this, std::placeholders::_1));

    m_controller.ErrorOccured.connect(
        std::bind(&ChatWindow::OnErrorMessage, this, std::placeholders::_1));

    m_ui->actionConnect->setEnabled(false);
    m_ui->actionDisconnect->setEnabled(false);
    m_ui->pushButton->setEnabled(false);

    move(QGuiApplication::screens().at(0)->geometry().center() - frameGeometry().center());
}

void ChatWindow::InitiateDisconnection()
{
    m_connectionErrorHandling.disconnect();
    m_controller.GetConnectionController().Disconnect();
    m_ui->listWidget->clear();
}

void ChatWindow::OnMessageReceived(const UserMessage& message)
{
    m_ui->textBrowser->setTextColor(Qt::darkBlue);
    m_ui->textBrowser->append((' ' + message.nickname + ':').c_str());

    m_ui->textBrowser->setTextColor(Qt::black);
    m_ui->textBrowser->append(message.messageText.c_str());
}

void ChatWindow::OnServiceMessageReceived(const std::string &message, Event event)
{
    switch (event)
    {
    case Event::ServiceMessagePayload_Event_UserConnected:
        m_ui->textBrowser->setTextColor(Qt::darkGreen);
        break;

    case Event::ServiceMessagePayload_Event_UserDisconnected:
        m_ui->textBrowser->setTextColor(Qt::darkMagenta);
        break;

    default:
        assert(!"Unsupported event type");
        break;

    }

    m_ui->textBrowser->append(message.c_str());
}

void ChatWindow::OnActiveUserListUpdated(const UserList& activeUsers)
{
    for(const auto& [key, value] : activeUsers)
    {
        if(value)
        {
            m_ui->listWidget->addItem(key.c_str());
        }
        else
        {
            auto items = m_ui->listWidget->findItems(key.c_str(), Qt::MatchExactly);
            assert(items.size() == 1);
            delete items[0];
        }
    }

    m_ui->listWidget->sortItems();
}

void ChatWindow::OnErrorMessage(const std::string &errText)
{
    QMessageBox errorBox(QMessageBox::Icon::Critical,
                         "Error",
                         errText.c_str(),
                         QMessageBox::Ok,
                         this);
    errorBox.exec();
}

void ChatWindow::ShowConnectionDialog()
{
    if(!m_connectionErrorHandling.connected())
        m_connectionErrorHandling = m_controller.GetConnectionController().ErrorOccured.connect(
                            std::bind(&ChatWindow::OnErrorMessage, this, std::placeholders::_1));

    std::string host;
    uint16_t port;
    LoginInfo loginInfo;

    ConnectionDialog connectionDialog (host, port, loginInfo, this);

    switch(connectionDialog.exec())
    {
    case ConnectionDialog::Cancel:
        close();
        exit(0);
        break;

    case ConnectionDialog::LogIn:
        m_controller.GetConnectionController().SignIn(host, port, loginInfo);
        break;

    case ConnectionDialog::Register:
        ShowRegistrationDialog(host, port);
        break;
    }
}

void ChatWindow::ShowRegistrationDialog(const std::string& host, uint16_t port)
{
    RegistrationInfo regInfo;
    RegistrationDialog registrationDialog(this);
    bool filled = registrationDialog.exec(regInfo);

    if(filled)
        m_controller.GetConnectionController().SignUp(host, port, regInfo);
}

ChatWindow::~ChatWindow()
{
    delete m_ui;
}

void ChatWindow::on_pushButton_clicked()
{
    Send();
}

void ChatWindow::Send()
{
    m_controller.SendMessage(m_ui->messageField->toPlainText().toStdString());
    m_ui->messageField->clear();
}

void ChatWindow::OnConnectionControlerStateChanged(ConnectionController::State state)
{
    switch (state)
    {
    case ConnectionController::State::Disconnected:
        m_ui->actionConnect->setEnabled(true);
        m_ui->actionDisconnect->setEnabled(false);
        m_ui->pushButton->setEnabled(false);
        m_ui->label->setText("State: Unconnected");
        break;

    case ConnectionController::State::EstablishingConnection:
    case ConnectionController::State::ConnectingServer:
        m_ui->actionConnect->setEnabled(false);
        m_ui->actionDisconnect->setEnabled(false);
        m_ui->pushButton->setEnabled(false);
        m_ui->label->setText("State: Connecting");
        break;

    case ConnectionController::State::LogInOrRegistering:
        m_ui->label->setText("State: Connecting");
        break;

    case ConnectionController::State::Connected:
        m_ui->actionConnect->setEnabled(false);
        m_ui->actionDisconnect->setEnabled(true);
        m_ui->pushButton->setEnabled(true);
        m_ui->label->setText("State: Connected");
        break;

    case ConnectionController::State::Disconnecting:
        m_ui->actionConnect->setEnabled(true);
        m_ui->actionDisconnect->setEnabled(false);
        m_ui->pushButton->setEnabled(false);
        m_ui->label->setText("State: Disconnecting");
        break;

    default:
        assert(!"Not Implemented");
    }
}

void ChatWindow::on_actionConnect_triggered()
{
    ShowConnectionDialog();
}

void ChatWindow::on_actionDisconnect_triggered()
{
    InitiateDisconnection();
}

void ChatWindow::on_actionQuit_triggered()
{
    if(m_ui->actionDisconnect->isEnabled())
        InitiateDisconnection();

    close();
}

