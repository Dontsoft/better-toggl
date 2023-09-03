#ifndef TOGGL_APIINTERFACE_HPP
#define TOGGL_APIINTERFACE_HPP

#include <optional>

#include <QDateTime>
#include <QJsonArray>
#include <QJsonObject>
#include <QObject>
#include <QResizeEvent>
#include <QSharedPointer>
#include <QStringList>

#include "apitypes.hpp"
#include "timeentrylist.hpp"

class QNetworkAccessManager;
class GlobalStorage;
class PersistentStorage;

namespace toggl {

    class ApiInterface : public QObject
    {
        Q_OBJECT
    public:
        explicit ApiInterface(QNetworkAccessManager *networkAccessManager,
                              QSharedPointer<PersistentStorage> persistentStorage,
                              const QString &sessionCookieName = QString(),
                              QObject *parent = nullptr);

        void setApiToken(const QString &token) { _apiToken = token; }

    public slots:
        void checkAuthenticationState();
        void auth(const QString &email, const QString &password, bool rememberMe = false);
        void authWithSessionCookie();
        void logout();
        void me(bool withRelatedData = false);
        void workspaces();
        void timeEntriesLatest();

        void reportUsers(int workspaceId);
        void reportClients(int workspaceId);
        void reportProjects(int workspaceId);
        void reportTasks(int workspaceId);

        void details(int workspaceId,
                     const QDate &startDate,
                     const QDate &endDate,
                     const QList<toggl::ReportUser> &users,
                     const QList<toggl::Client> &clients,
                     const QList<toggl::Project> &projects,
                     const QList<toggl::Task> &tasks);

    signals:
        void error(const QString &error);
        void checkAuthenticationStateAnswer(bool isAuthenticated);
        void authAnswer(bool success, const QString &errorMessage = QString());
        void logoutAnswer();
        void authWithSessionCookieAnswer(bool success);
        void meAnswer(const Me &me);
        void workspacesAnswer(const QList<Workspace> &workspaces);
        void timeEntriesLatestAnswer(const TimeEntryList &timeEntries);

        void checkIfSessionIsValidAnswer(bool isValid);

        void reportUsersAnswer(const QList<toggl::ReportUser> &users);
        void reportClientsAnswer(const QList<toggl::Client> &clients);
        void reportProjectsAnswer(const QList<toggl::Project> &projects);
        void reportTasksAnswer(const QList<toggl::Task> &tasks);
        void detailsAnswer(const TimeEntryList &timeEntries);

    protected:
        QNetworkAccessManager *networkAccessManager() const { return _networkAccessManager; }

    protected slots:
        void checkIfSessionIsValid();

    private:
        QString getSessionCookie() const;
        void setSessionCookie(const QString &cookie);

        QNetworkAccessManager *_networkAccessManager;
        QSharedPointer<PersistentStorage> _persistentStorage;

        const QString _sessionCookieName;

        bool _wasRememberMe = false;

        inline bool useSession() const { return _apiToken.isEmpty(); }
        QString _apiToken;
    };

} // namespace toggl

#endif // TOGGL_APIINTERFACE_HPP
