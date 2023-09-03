#include "logoutdialog.hpp"
#include "ui_logoutdialog.h"

#include <QPushButton>

LogoutDialog::LogoutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogoutDialog)
{
    ui->setupUi(this);
    connect(ui->cancelButton, &QAbstractButton::clicked, this, &QDialog::reject);
    connect(ui->logoutButton, &QAbstractButton::clicked, this, &QDialog::accept);
    ui->cancelButton->setDefault(false);
    ui->logoutButton->setDefault(true);
}

LogoutDialog::~LogoutDialog()
{
    delete ui;
}
