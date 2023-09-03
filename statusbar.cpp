#include "statusbar.hpp"
#include "ui_statusbar.h"

StatusBar::StatusBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StatusBar)
{
    ui->setupUi(this);
}

StatusBar::~StatusBar()
{
    delete ui;
}

void StatusBar::updateWorkspace(const QString &workspaceName, const QString &workspaceImageUrl)
{
    ui->workspaceNameLabel->setText(workspaceName);
    ui->workspaceImageViewer->setImageUrl(workspaceImageUrl);
}

void StatusBar::updateUser(const QString &name, const QString &userImageUrl)
{
    ui->userNameLabel->setText(name);
    ui->userImageViewer->setImageUrl(userImageUrl);
}

void StatusBar::setGlobalStorageToImageViewer(QSharedPointer<GlobalStorage> globalStorage)
{
    ui->workspaceImageViewer->setGlobalStorage(globalStorage);
    ui->userImageViewer->setGlobalStorage(globalStorage);
}
