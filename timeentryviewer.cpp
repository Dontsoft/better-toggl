#include "timeentryviewer.hpp"
#include "ui_timeentryviewer.h"

#include <QDebug>

TimeEntryViewer::TimeEntryViewer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TimeEntryViewer)
{
    ui->setupUi(this);
}

TimeEntryViewer::~TimeEntryViewer()
{
    delete ui;
}

void TimeEntryViewer::setTimeEntryList(const toggl::TimeEntryList &newTimeEntryList)
{
    _timeEntryList = newTimeEntryList;
    QStringList columnHeaders{tr("Client"),
                              tr("Project"),
                              tr("Task"),
                              tr("User"),
                              tr("Duration"),
                              tr("Time")};
    ui->timeEntryTable->setColumnCount(columnHeaders.count());
    ui->timeEntryTable->setHorizontalHeaderLabels(columnHeaders);
    qInfo() << _timeEntryList.count();
    for (const auto &entry : _timeEntryList) {
        ui->timeEntryTable->insertRow(ui->timeEntryTable->rowCount());
        auto project = getProject(entry.get<int>(toggl::TimeEntry::PROJECT_ID).value_or(-1));
        auto client = getClient(project.value().get<int>(toggl::Project::CLIENT_ID).value_or(-1));
        auto task = getTask(entry.get<int>(toggl::TimeEntry::TASK_ID).value_or(-1));
        auto user = getUser(entry.get<int>(toggl::TimeEntry::USER_ID).value_or(-1));
        auto duration = entry.get<int>(toggl::TimeEntry::DURATION).value_or(0);
        auto start = entry.get<QDateTime>(toggl::TimeEntry::START).value_or(QDateTime());
        auto stop = entry.get<QDateTime>(toggl::TimeEntry::STOP).value_or(QDateTime());

        ui->timeEntryTable->setItem(ui->timeEntryTable->rowCount() - 1,
                                    0,
                                    new QTableWidgetItem(
                                        client.value().get<QString>(toggl::Client::NAME).value()));
        ui->timeEntryTable->setItem(ui->timeEntryTable->rowCount() - 1,
                                    1,
                                    new QTableWidgetItem(
                                        project.value().get<QString>(toggl::Project::NAME).value()));
        ui->timeEntryTable->setItem(ui->timeEntryTable->rowCount() - 1,
                                    2,
                                    new QTableWidgetItem(
                                        task.value().get<QString>(toggl::Task::NAME).value()));
        ui->timeEntryTable
            ->setItem(ui->timeEntryTable->rowCount() - 1,
                      3,
                      new QTableWidgetItem(
                          user.value().get<QString>(toggl::ReportUser::FULLNAME).value()));

        int _seconds = duration;
        int hours = _seconds / 3600;
        _seconds = _seconds % 3600;
        int minutes = _seconds / 60;
        _seconds = _seconds % 60;
        auto durationString = QString("%1:%2:%3")
                                  .arg(hours, ((hours >= 100) ? 0 : 2), 10, QChar('0'))
                                  .arg(minutes, 2, 10, QChar('0'))
                                  .arg(_seconds, 2, 10, QChar('0'));

        ui->timeEntryTable->setItem(ui->timeEntryTable->rowCount() - 1,
                                    4,
                                    new QTableWidgetItem(durationString));
        ui->timeEntryTable->setItem(ui->timeEntryTable->rowCount() - 1,
                                    5,
                                    new QTableWidgetItem(
                                        QString("%1 - %2").arg(start.toString(), stop.toString())));
    }
    for (int i = 0; i < columnHeaders.count(); ++i) {
        ui->timeEntryTable->resizeColumnToContents(i);
    }
}

void TimeEntryViewer::setProjects(const QList<toggl::Project> &newProjects)
{
    _projects = newProjects;
}

void TimeEntryViewer::setTasks(const QList<toggl::Task> &newTasks)
{
    _tasks = newTasks;
}

void TimeEntryViewer::setUsers(const QList<toggl::ReportUser> &newUsers)
{
    _users = newUsers;
}

void TimeEntryViewer::setClients(const QList<toggl::Client> &newClients)
{
    _clients = newClients;
}

template<typename T, template<typename> class Container>
std::optional<T> lookupId(int id, const Container<T> &container, const QString &key)
{
    auto it = std::find_if(std::begin(container), std::end(container), [=](const T &entry) {
        return entry.template get<int>(key).value_or(-1) == id;
    });
    if (it == std::end(container)) {
        return std::nullopt;
    }
    return (*it);
}

std::optional<toggl::Client> TimeEntryViewer::getClient(int clientId) const
{
    return lookupId(clientId, _clients, toggl::Client::ID);
}

std::optional<toggl::Project> TimeEntryViewer::getProject(int projectId) const
{
    return lookupId(projectId, _projects, toggl::Project::ID);
}

std::optional<toggl::ReportUser> TimeEntryViewer::getUser(int userId) const
{
    return lookupId(userId, _users, toggl::ReportUser::ID);
}

std::optional<toggl::Task> TimeEntryViewer::getTask(int taskId) const
{
    return lookupId(taskId, _tasks, toggl::Task::ID);
}
