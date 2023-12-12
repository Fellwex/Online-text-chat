#include "registration_dialog.h"
#include "ui_registration_dialog.h"

#include <common/credential_validator.h>

#include <QPushButton>

RegistrationDialog::RegistrationDialog(QWidget *parent)
    : QDialog(parent)
    , m_ui(new Ui::RegistrationDialog)
{
    m_ui->setupUi(this);
    m_ui->incorrectUserInput->setStyleSheet("QLabel {color: red;}");

    move(QGuiApplication::screens().at(0)->geometry().center() - frameGeometry().center());

    m_ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    connect(m_ui->loginField, SIGNAL(textChanged(QString)),
            this, SLOT(OnLoginOrPasswordTextChanged()));
    connect(m_ui->passwordField, SIGNAL(textChanged(QString)),
            this, SLOT(OnLoginOrPasswordTextChanged()));
    connect(m_ui->passwordConfField, SIGNAL(textChanged(QString)),
            this, SLOT(OnLoginOrPasswordTextChanged()));
}

RegistrationDialog::~RegistrationDialog()
{
    delete m_ui;
}

bool RegistrationDialog::exec(RegistrationInfo& regInfo)
{
    bool accepted = QDialog::exec();

    if (accepted)
    {
        regInfo.login = m_ui->loginField->text().toStdString();
        regInfo.password = m_ui->passwordField->text().toStdString();
        regInfo.passwordConfirmation = m_ui->passwordConfField->text().toStdString();
    }
    return accepted;
}

void RegistrationDialog::OnLoginOrPasswordTextChanged()
{
    std::string errorString;

    if(!CredentialValidator::CheckLogin(m_ui->loginField->text().toStdString(), errorString))
        m_ui->incorrectUserInput->setText(errorString.c_str());
    else if(!CredentialValidator::CheckPassword(m_ui->passwordField->text().toStdString(), errorString))
        m_ui->incorrectUserInput->setText(errorString.c_str());
    else if(m_ui->passwordField->text() != m_ui->passwordConfField->text())
        m_ui->incorrectUserInput->setText("The passwords do not match");
    else
        m_ui->incorrectUserInput->setText("");

    m_ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(m_ui->incorrectUserInput->text().isEmpty());
}
