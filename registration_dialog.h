#pragma once

#include <QDialog>
#include <QScreen>
#include "registration_info.h"

namespace Ui {
class RegistrationDialog;
}

class RegistrationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegistrationDialog(QWidget *parent = nullptr);
    ~RegistrationDialog();

    bool exec(RegistrationInfo& regInfo);

private slots:
    void OnLoginOrPasswordTextChanged();

private:
    Ui::RegistrationDialog *m_ui;
};
