#include "globalstorage.hpp"

const QString GlobalStorage::CURRENT_BILLING_MONTH = QStringLiteral("CurrentBillingMonth");
const QString GlobalStorage::ME = QStringLiteral("Me");
const QString GlobalStorage::WORKSPACES = QStringLiteral("Workspaces");
const QString GlobalStorage::LATEST_TIME_ENTRIES = QStringLiteral("LatestTimeEntries");

const QString GlobalStorage::USERS = QStringLiteral("Users");
const QString GlobalStorage::CLIENTS = QStringLiteral("Clients");
const QString GlobalStorage::PROJECTS = QStringLiteral("Projects");
const QString GlobalStorage::TASKS = QStringLiteral("Tasks");
const QString GlobalStorage::BILLING_SUMMARIZER = QStringLiteral("BillingSummarizer");
const QString GlobalStorage::DETAILS = QStringLiteral("Details");

GlobalStorage::GlobalStorage(QObject* parent)
    : QObject{parent}
{

}

void GlobalStorage::set(const QString& key, const QVariant& value, bool emitNotify)
{
    const auto currentValue = _storage.value(key);
    _storage.insert(key, value);
    if (currentValue != value && emitNotify) {
        emit notify(key);
    }
}

void GlobalStorage::set(const toggl::Me& me, bool emitNotify)
{
    _me = me;
    if (emitNotify) {
        emit notify(GlobalStorage::ME);
    }
}

void GlobalStorage::set(const QList<toggl::Workspace>& workspaces, bool emitNotify)
{
    _workspaces = workspaces;
    if (emitNotify) {
        emit notify(GlobalStorage::WORKSPACES);
    }
}

void GlobalStorage::set(const toggl::TimeEntryList& timeEntries, bool emitNotify)
{
    _latestTimeEntries = timeEntries;
    if (emitNotify) {
        emit notify(GlobalStorage::LATEST_TIME_ENTRIES);
    }
}

void GlobalStorage::set(const QList<toggl::ReportUser>& users, bool emitNotify)
{
    _users = users;
    if (emitNotify) {
        emit notify(GlobalStorage::USERS);
    }
}

void GlobalStorage::set(const QList<toggl::Client>& clients, bool emitNotify)
{
    _clients = clients;
    if (emitNotify) {
        emit notify(GlobalStorage::CLIENTS);
    }
}

void GlobalStorage::set(const QList<toggl::Project>& projects, bool emitNotify)
{
    _projects = projects;
    if (emitNotify) {
        emit notify(GlobalStorage::PROJECTS);
    }
}

void GlobalStorage::set(const QList<toggl::Task>& tasks, bool emitNotify)
{
    _tasks = tasks;
    if (emitNotify) {
        emit notify(GlobalStorage::TASKS);
    }
}

void GlobalStorage::set(const QMap<int, BillingSummarizer>& billingSummarizer, bool emitNotify)
{
    _billingSummarizer = billingSummarizer;
    if (emitNotify) {
        emit notify(GlobalStorage::BILLING_SUMMARIZER);
    }
}

void GlobalStorage::clear()
{
    _storage.clear();
    _me = toggl::Me();
    _workspaces.clear();
    _latestTimeEntries.clear();
    _users.clear();
    _clients.clear();
    _projects.clear();
    _tasks.clear();
}

bool GlobalStorage::has(const QString& key) const
{
    return _storage.contains(key);
}

QVariant GlobalStorage::get(const QString& key, const QVariant& defaultValue) const
{
    return _storage.value(key, defaultValue);
}

toggl::Me GlobalStorage::me() const
{
    return _me;
}

QList<toggl::Workspace> GlobalStorage::workspaces() const
{
    return _workspaces;
}

std::optional<toggl::Workspace> GlobalStorage::workspace(int id) const
{
    auto it = std::find_if(std::begin(_workspaces),
                           std::end(_workspaces),
                           [=](const toggl::Workspace& workspace) {
                               return workspace.get<int>(toggl::Workspace::ID) == id;
                           });
    if (it == std::end(_workspaces)) {
        return std::nullopt;
    }
    return *it;
}

toggl::TimeEntryList GlobalStorage::latestTimeEntries() const
{
    return _latestTimeEntries;
}

QList<toggl::ReportUser> GlobalStorage::users() const
{
    return _users;
}

QList<toggl::Client> GlobalStorage::clients() const
{
    return _clients;
}

QList<toggl::Project> GlobalStorage::projects() const
{
    return _projects;
}

QList<toggl::Task> GlobalStorage::tasks() const
{
    return _tasks;
}

QMap<int, BillingSummarizer> GlobalStorage::billingSummarizer() const
{
    return _billingSummarizer;
}
