#include "apilogindialog.hpp"
#include "ui_apilogindialog.h"

#include <QPushButton>

#include "simpleconfig.hpp"
#include "toggl/apiinterface.hpp"
ApiLoginDialog::ApiLoginDialog(QSharedPointer<toggl::ApiInterface> apiInterface,
                               QSharedPointer<SimpleConfig> simpleConfig,
                               QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ApiLoginDialog)
    , _apiInterface(apiInterface)
    , _simpleConfig(simpleConfig)
{
    ui->setupUi(this);
    ui->loginButtonBox->button(QDialogButtonBox::Ok)->setText(tr("Login"));
    ui->busyIndicator->hide();
    ui->errorLabel->hide();
    connect(ui->loginButtonBox, &QDialogButtonBox::clicked, this, &ApiLoginDialog::buttonClicked);
    connect(_apiInterface.data(),
            &toggl::ApiInterface::authAnswer,
            this,
            &ApiLoginDialog::authAnswer);
    QByteArray existingApiToken = _simpleConfig->getEncrypted(SimpleConfig::API_TOKEN);
    if (!existingApiToken.isEmpty()) {
        ui->apiTokenEdit->setText(QString::fromUtf8(existingApiToken));
        loginClicked();
    }
}

ApiLoginDialog::~ApiLoginDialog()
{
    delete ui;
}

void ApiLoginDialog::buttonClicked(QAbstractButton *button)
{
    if (button == ui->loginButtonBox->button(QDialogButtonBox::Ok)) {
        loginClicked();
    } else {
        reject();
    }
}

void ApiLoginDialog::loginClicked()
{
    ui->busyIndicator->show();
    ui->loginButtonBox->hide();
    _apiInterface->auth(ui->apiTokenEdit->text(), "api_token", false);
}
void ApiLoginDialog::authAnswer(bool success, const QString &errorText)
{
    if (success) {
        _simpleConfig->setEncrypted(SimpleConfig::API_TOKEN, ui->apiTokenEdit->text().toUtf8());
        //_apiInterface->setApiToken(ui->apiTokenEdit->text());
        accept();
    } else {
        ui->busyIndicator->hide();
        ui->loginButtonBox->show();
        ui->errorLabel->show();
        ui->errorLabel->setText(errorText);
    }
}
