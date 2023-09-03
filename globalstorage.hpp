#ifndef GLOBALSTORAGE_HPP
#define GLOBALSTORAGE_HPP

#include <QObject>
#include <QVariantMap>
#include "billingsummarizer.hpp"
#include "toggl/apitypes.hpp"
#include "toggl/timeentrylist.hpp"

class GlobalStorage : public QObject
{
    Q_OBJECT
public:
    static const QString CURRENT_BILLING_MONTH;
    static const QString ME;
    static const QString WORKSPACES;
    static const QString LATEST_TIME_ENTRIES;
    static const QString USERS;
    static const QString CLIENTS;
    static const QString PROJECTS;
    static const QString TASKS;
    static const QString BILLING_SUMMARIZER;
    static const QString DETAILS;

    explicit GlobalStorage(QObject* parent = nullptr);
    bool has(const QString& key) const;
    QVariant get(const QString& key, const QVariant& defaultValue = QVariant()) const;

    toggl::Me me() const;

    QList<toggl::Workspace> workspaces() const;
    std::optional<toggl::Workspace> workspace(int id) const;

    toggl::TimeEntryList latestTimeEntries() const;
    QList<toggl::ReportUser> users() const;
    QList<toggl::Client> clients() const;
    QList<toggl::Project> projects() const;
    QList<toggl::Task> tasks() const;
    QMap<int, BillingSummarizer> billingSummarizer() const;
public slots:

    void set(const QString& key, const QVariant& value) { set(key, value, true); }
    void set(const toggl::Me& me) { set(me, true); }
    void set(const QList<toggl::Workspace>& workspaces) { set(workspaces, true); }
    void set(const toggl::TimeEntryList& timeEntries) { set(timeEntries, true); }
    void set(const QList<toggl::ReportUser>& users) { set(users, true); }
    void set(const QList<toggl::Client>& clients) { set(clients, true); }
    void set(const QList<toggl::Project>& projects) { set(projects, true); }
    void set(const QList<toggl::Task>& tasks) { set(tasks, true); }
    void set(const QMap<int, BillingSummarizer>& billingSummarizer)
    {
        set(billingSummarizer, true);
    }

    void set(const QString& key, const QVariant& value, bool emitNotify);
    void set(const toggl::Me& me, bool emitNotify);
    void set(const QList<toggl::Workspace>& workspaces, bool emitNotify);
    void set(const toggl::TimeEntryList& timeEntries, bool emitNotify);
    void set(const QList<toggl::ReportUser>& users, bool emitNotify);
    void set(const QList<toggl::Client>& clients, bool emitNotify);
    void set(const QList<toggl::Project>& projects, bool emitNotify);
    void set(const QList<toggl::Task>& tasks, bool emitNotify);
    void set(const QMap<int, BillingSummarizer>& billingSummarizer, bool emitNotify);

    void clear();
signals:
    void notify(const QString& key);

private:
    QVariantMap _storage;
    toggl::Me _me;
    QList<toggl::Workspace> _workspaces;
    toggl::TimeEntryList _latestTimeEntries;
    QList<toggl::ReportUser> _users;
    QList<toggl::Client> _clients;
    QList<toggl::Project> _projects;
    QList<toggl::Task> _tasks;
    QMap<int, BillingSummarizer> _billingSummarizer;
};

#endif // GLOBALSTORAGE_HPP
