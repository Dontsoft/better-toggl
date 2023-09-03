#include "timeentryviewerdialog.hpp"
#include "ui_timeentryviewerdialog.h"

TimeEntryViewerDialog::TimeEntryViewerDialog(const toggl::TimeEntryList &list,
                                             const QList<toggl::Client> &clients,
                                             const QList<toggl::Project> &projects,
                                             const QList<toggl::Task> &tasks,
                                             const QList<toggl::ReportUser> &user,
                                             QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::TimeEntryViewerDialog)
{
    ui->setupUi(this);
    setWindowTitle(tr("Time Entry Viewer"));
    ui->timeEntryViewer->setClients(clients);
    ui->timeEntryViewer->setProjects(projects);
    ui->timeEntryViewer->setTasks(tasks);
    ui->timeEntryViewer->setUsers(user);
    ui->timeEntryViewer->setTimeEntryList(list);
    connect(ui->closeButton, &QPushButton::clicked, this, &QDialog::accept);
}

TimeEntryViewerDialog::~TimeEntryViewerDialog()
{
    delete ui;
}
