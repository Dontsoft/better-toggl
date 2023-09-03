#include "logindialog.hpp"

#include <QPushButton>

#include "persistentstorage.hpp"
#include "toggl/apiinterface.hpp"
#include "ui_logindialog.h"

LoginDialog::LoginDialog(QSharedPointer<toggl::ApiInterface> apiInterface,
                         QSharedPointer<PersistentStorage> persistentStorage,
                         QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
    , _apiInterface(apiInterface)
    , _persistentStorage(persistentStorage)
{
    ui->setupUi(this);
    ui->loginButtonBox->button(QDialogButtonBox::Ok)->setText(tr("Login"));
    ui->busyIndicator->hide();
    ui->errorLabel->hide();
    ui->emailEdit->setFocus();
    connect(ui->loginButtonBox, &QDialogButtonBox::clicked, this, &LoginDialog::buttonClicked);
    connect(_apiInterface.data(), &toggl::ApiInterface::authAnswer, this, &LoginDialog::authAnswer);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::buttonClicked(QAbstractButton *button)
{
    if (button == ui->loginButtonBox->button(QDialogButtonBox::Ok)) {
        loginClicked();
    } else {
        reject();
    }
}

void LoginDialog::loginClicked()
{
    ui->busyIndicator->show();
    ui->loginButtonBox->hide();
    _apiInterface->auth(ui->emailEdit->text(),
                        ui->passwordEdit->text(),
                        ui->rememberMeCheckBox->isChecked());
}
void LoginDialog::authAnswer(bool success, const QString &errorText)
{
    if (success) {
        accept();
    } else {
        ui->busyIndicator->hide();
        ui->loginButtonBox->show();
        ui->errorLabel->show();
        ui->errorLabel->setText(errorText);
    }
}
