#include "connection_dialog.h"
#include "ui_connection_dialog.h"

#include <common/credential_validator.h>

#include <QMessageBox>

ConnectionDialog::ConnectionDialog(std::string& host, uint16_t& port, LoginInfo& loginInfo, QWidget *parent)
    : m_host(host)
    , m_port(port)
    , m_loginInfo(loginInfo)
    , QDialog(parent)
    , m_ui(new Ui::ConnectionDialog)
{
    m_ui->setupUi(this);

    move(QGuiApplication::screens().at(0)->geometry().center() - frameGeometry().center());
    m_ui->incorrectUserInput->setStyleSheet("QLabel {color: red;}");

    m_ui->loginButton->setEnabled(false);
    connect(m_ui->loginField, SIGNAL(textChanged(QString)),
            this, SLOT(OnLoginOrPasswordTextChanged()));
    connect(m_ui->passwordField, SIGNAL(textChanged(QString)),
            this, SLOT(OnLoginOrPasswordTextChanged()));
}

ConnectionDialog::~ConnectionDialog()
{
    delete m_ui;
}
void ConnectionDialog::on_loginButton_clicked()
{
    m_host = m_ui->hostField->text().toStdString();
    m_port = m_ui->portField->text().toUShort();

    m_loginInfo.login = m_ui->loginField->text().toStdString();
    m_loginInfo.password = m_ui->passwordField->text().toStdString();

    done(ReturnValue::LogIn);
}

void ConnectionDialog::on_registerButton_clicked()
{
    m_host = m_ui->hostField->text().toStdString();
    m_port = m_ui->portField->text().toUShort();

    done(ReturnValue::Register);
}

void ConnectionDialog::on_quitButton_clicked()
{
    done(ReturnValue::Cancel);
}

void ConnectionDialog::OnLoginOrPasswordTextChanged()
{
    std::string errorString;

    if(!CredentialValidator::CheckLogin(m_ui->loginField->text().toStdString(), errorString))
        m_ui->incorrectUserInput->setText(errorString.c_str());
    else if(!CredentialValidator::CheckPassword(m_ui->passwordField->text().toStdString(), errorString))
        m_ui->incorrectUserInput->setText(errorString.c_str());
    else
        m_ui->incorrectUserInput->setText("");

    m_ui->loginButton->setEnabled(m_ui->incorrectUserInput->text().isEmpty());
}

