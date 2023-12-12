#pragma once

#include <QDialog>
#include "connection_controller.h"
#include "common/utils/connection_remover.h"
#include "registration_dialog.h"

namespace bs = boost::signals2;

namespace Ui {
class ConnectionDialog;
}

class ConnectionDialog : public QDialog
{
    Q_OBJECT

public:
    enum ReturnValue
    {
        Cancel,
        Register,
        LogIn
    };

    explicit ConnectionDialog(std::string& host, uint16_t& port, LoginInfo& loginInfo, QWidget *parent = nullptr);
    ~ConnectionDialog();

private slots:
    void on_loginButton_clicked();
    void on_registerButton_clicked();
    void on_quitButton_clicked();
    void OnLoginOrPasswordTextChanged();

private:
    std::string& m_host;
    uint16_t& m_port;
    LoginInfo& m_loginInfo;
    Ui::ConnectionDialog *m_ui;
};
