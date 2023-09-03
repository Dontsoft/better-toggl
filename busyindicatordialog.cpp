#include "busyindicatordialog.hpp"
#include "ui_busyindicatordialog.h"

BusyIndicatorDialog::BusyIndicatorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BusyIndicatorDialog)
{
    ui->setupUi(this);
    setWindowTitle(tr("Busy ..."));
    setWindowFlags(Qt::Tool | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
}

BusyIndicatorDialog::~BusyIndicatorDialog()
{
    delete ui;
}

void BusyIndicatorDialog::setIndicatorMessage(const QString &message)
{
    if (message.isEmpty()) {
        ui->busyIndicatorMessageLabel->hide();
    } else {
        ui->busyIndicatorMessageLabel->setText(message);
        ui->busyIndicatorMessageLabel->show();
        setWindowTitle(message);
    }
}
