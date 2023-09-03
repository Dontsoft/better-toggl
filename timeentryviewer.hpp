#ifndef TIMEENTRYVIEWER_HPP
#define TIMEENTRYVIEWER_HPP

#include <QWidget>

#include "toggl/timeentrylist.hpp"

namespace Ui {
    class TimeEntryViewer;
}

class TimeEntryViewer : public QWidget
{
    Q_OBJECT

public:
    explicit TimeEntryViewer(QWidget *parent = nullptr);
    ~TimeEntryViewer();

    void setTimeEntryList(const toggl::TimeEntryList &newTimeEntryList);

    void setProjects(const QList<toggl::Project> &newProjects);

    void setTasks(const QList<toggl::Task> &newTasks);

    void setUsers(const QList<toggl::ReportUser> &newUsers);

    void setClients(const QList<toggl::Client> &newClients);

private:
    Ui::TimeEntryViewer *ui;

    std::optional<toggl::Client> getClient(int clientId) const;
    std::optional<toggl::Project> getProject(int projectId) const;
    std::optional<toggl::ReportUser> getUser(int userId) const;
    std::optional<toggl::Task> getTask(int taskId) const;

    toggl::TimeEntryList _timeEntryList;
    QList<toggl::Project> _projects;
    QList<toggl::Task> _tasks;
    QList<toggl::ReportUser> _users;
    QList<toggl::Client> _clients;
};

#endif // TIMEENTRYVIEWER_HPP
