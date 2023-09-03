#include "apiinterface.hpp"

#include <algorithm>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QNetworkAccessManager>
#include <QNetworkCookie>
#include <QNetworkCookieJar>
#include <QNetworkReply>
#include <QUrl>

#include <QDebug>

#include "apiendpoints.hpp"
#include "persistentstorage.hpp"

namespace toggl {

    static const QString SESSION_COOKIE = QStringLiteral("__Host-timer-session");

    ApiInterface::ApiInterface(QNetworkAccessManager *networkAccessManager,
                               QSharedPointer<PersistentStorage> persistentStorage,
                               const QString &sessionCookieName,
                               QObject *parent)
        : QObject{parent}
        , _networkAccessManager(networkAccessManager)
        , _persistentStorage(persistentStorage)
        , _sessionCookieName(sessionCookieName.isEmpty() ? SESSION_COOKIE : sessionCookieName)
    {
        const auto cookie = _persistentStorage->getCookie(_sessionCookieName);
        if (!cookie.isEmpty()) {
            _wasRememberMe = true;
            setSessionCookie(cookie);
        }
    }

    QString ApiInterface::getSessionCookie() const
    {
        const auto cookieJar = _networkAccessManager->cookieJar();
        const auto allTogglCookies = cookieJar->cookiesForUrl(QUrl(toggl::API_BASEURL));
        auto it = std::find_if(std::begin(allTogglCookies),
                               std::end(allTogglCookies),
                               [](const QNetworkCookie cookie) {
                                   return cookie.name() == SESSION_COOKIE;
                               });
        if (it == std::end(allTogglCookies)) {
            return QString();
        }
        return (*it).toRawForm();
    }

    void ApiInterface::setSessionCookie(const QString &cookie)
    {
        auto cookieJar = _networkAccessManager->cookieJar();
        cookieJar->setCookiesFromUrl(QNetworkCookie::parseCookies(cookie.toUtf8()),
                                     QUrl(toggl::API_BASEURL));
        _networkAccessManager->setCookieJar(cookieJar);
    }

    void ApiInterface::checkAuthenticationState()
    {
        const auto cookie = getSessionCookie();
        if (cookie.isEmpty()) {
            emit checkAuthenticationStateAnswer(false);
        } else {
            checkIfSessionIsValid();
        }
    }

    void ApiInterface::checkIfSessionIsValid()
    {
        auto conn = QSharedPointer<QMetaObject::Connection>::create();
        *conn = connect(this, &ApiInterface::authWithSessionCookieAnswer, this, [=](bool success) {
            disconnect(*conn);
            emit checkAuthenticationStateAnswer(success);
        });
        authWithSessionCookie();
    }

    void ApiInterface::auth(const QString &email, const QString &password, bool rememberMe)
    {
        _wasRememberMe = rememberMe;
        QJsonObject body;
        body["remember_me"] = rememberMe;
        QJsonDocument doc(body);
        qInfo() << doc;
        QNetworkRequest request((QUrl(toggl::v9::SESSIONS)));

        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        request.setRawHeader("Authorization",
                             "Basic "
                                 + QString("%1:%2").arg(email, password).toLocal8Bit().toBase64());
        auto networkReply = _networkAccessManager->post(request, doc.toJson());
        connect(networkReply, &QNetworkReply::finished, this, [=]() {
            if (networkReply->error() == QNetworkReply::NoError) {
                if (rememberMe) {
                    _persistentStorage->setCookie(_sessionCookieName, getSessionCookie());
                }
                qInfo() << getSessionCookie();
                emit authAnswer(true);
            } else {
                emit authAnswer(false,
                                tr("Error while logging in. Please check email and password."));
            }
            networkReply->deleteLater();
        });
    }

    void ApiInterface::authWithSessionCookie()
    {
        QNetworkRequest request((QUrl(toggl::v9::SESSIONS)));

        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        auto networkReply = _networkAccessManager->post(request, QByteArray());
        connect(networkReply, &QNetworkReply::finished, this, [=]() {
            if (networkReply->error() == QNetworkReply::NoError) {
                if (_wasRememberMe) {
                    _persistentStorage->setCookie(_sessionCookieName, getSessionCookie());
                }
                emit authWithSessionCookieAnswer(true);
            } else {
                emit authWithSessionCookieAnswer(false);
            }
            networkReply->deleteLater();
        });
    }

    void ApiInterface::logout()
    {
        QNetworkRequest request((QUrl(toggl::v9::SESSIONS)));

        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        auto networkReply = _networkAccessManager->deleteResource(request);
        connect(networkReply, &QNetworkReply::finished, this, [=]() {
            _persistentStorage->deleteCookie(_sessionCookieName);
            auto cookieJar = _networkAccessManager->cookieJar();
            const auto allTogglCookies = cookieJar->cookiesForUrl(QUrl(toggl::API_BASEURL));
            auto it = std::find_if(std::begin(allTogglCookies),
                                   std::end(allTogglCookies),
                                   [](const QNetworkCookie cookie) {
                                       return cookie.name() == SESSION_COOKIE;
                                   });
            if (it != std::end(allTogglCookies)) {
                cookieJar->deleteCookie((*it));
                _networkAccessManager->setCookieJar(cookieJar);
            }
            emit logoutAnswer();
            networkReply->deleteLater();
        });
    }

    void ApiInterface::me(bool withRelatedData)
    {
        QNetworkRequest request(
            (QUrl(toggl::v9::ME + (withRelatedData ? "?with_related_data=true" : ""))));
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        if (!useSession()) {
            request
                .setRawHeader("Authorization",
                              "Basic "
                                  + QString("%1:api_token").arg(_apiToken).toLocal8Bit().toBase64());
        }
        auto networkReply = _networkAccessManager->get(request);
        connect(networkReply, &QNetworkReply::finished, this, [=]() {
            if (networkReply->error() == QNetworkReply::NoError) {
                auto responseData = networkReply->readAll();
                QJsonDocument doc = QJsonDocument::fromJson(responseData);
                Me me(doc.object());
                emit meAnswer(me);
            } else {
                qWarning() << networkReply->error() << networkReply->errorString();
            }
            networkReply->deleteLater();
        });
    }

    void ApiInterface::workspaces()
    {
        QNetworkRequest request((QUrl(toggl::v9::WORKSPACES)));
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        if (!useSession()) {
            request
                .setRawHeader("Authorization",
                              "Basic "
                                  + QString("%1:api_token").arg(_apiToken).toLocal8Bit().toBase64());
        }
        auto networkReply = _networkAccessManager->get(request);
        connect(networkReply, &QNetworkReply::finished, this, [=]() {
            if (networkReply->error() == QNetworkReply::NoError) {
                auto responseData = networkReply->readAll();
                QJsonDocument doc = QJsonDocument::fromJson(responseData);
                if (doc.isArray()) {
                    QList<Workspace> workspaces;
                    auto array = doc.array();
                    for (auto v : array) {
                        workspaces.append(Workspace(v.toObject()));
                    }
                    emit workspacesAnswer(workspaces);
                }
            } else {
                qWarning() << networkReply->error() << networkReply->errorString();
            }
            networkReply->deleteLater();
        });
    }

    void ApiInterface::timeEntriesLatest()
    {
        const auto now = QDateTime::currentDateTime();
        const auto latest = now - std::chrono::hours(24 * 31);
        QNetworkRequest request(
            (QUrl(toggl::v9::TIME_ENTRIES + QString("?since=%1").arg(latest.toSecsSinceEpoch()))));
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        if (!useSession()) {
            request
                .setRawHeader("Authorization",
                              "Basic "
                                  + QString("%1:api_token").arg(_apiToken).toLocal8Bit().toBase64());
        }
        auto networkReply = _networkAccessManager->get(request);
        connect(networkReply, &QNetworkReply::finished, this, [=]() {
            if (networkReply->error() == QNetworkReply::NoError) {
                auto responseData = networkReply->readAll();
                QJsonDocument doc = QJsonDocument::fromJson(responseData);
                if (doc.isArray()) {
                    TimeEntryList timeEntries;
                    auto array = doc.array();
                    for (auto v : array) {
                        timeEntries.append(TimeEntry(v.toObject()));
                    }
                    emit timeEntriesLatestAnswer(timeEntries);
                }
            } else {
                qWarning() << networkReply->error() << networkReply->errorString();
            }
            networkReply->deleteLater();
        });
    }

    void ApiInterface::reportUsers(int workspaceId)
    {
        QNetworkRequest request((QUrl(toggl::reports::v3::USERS.arg(workspaceId))));
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        if (!useSession()) {
            request
                .setRawHeader("Authorization",
                              "Basic "
                                  + QString("%1:api_token").arg(_apiToken).toLocal8Bit().toBase64());
        }
        QJsonObject body;
        body["start"] = 0;
        QJsonDocument doc(body);
        auto networkReply = _networkAccessManager->post(request, doc.toJson());
        connect(networkReply, &QNetworkReply::finished, this, [=]() {
            if (networkReply->error() == QNetworkReply::NoError) {
                auto responseData = networkReply->readAll();
                QJsonDocument doc = QJsonDocument::fromJson(responseData);
                if (doc.isArray()) {
                    QList<toggl::ReportUser> users;
                    auto array = doc.array();
                    for (auto v : array) {
                        users.append(toggl::ReportUser(v.toObject()));
                    }
                    qInfo() << "Users" << users.length();
                    emit reportUsersAnswer(users);
                }
            } else {
                qWarning() << networkReply->error() << networkReply->errorString();
            }
            networkReply->deleteLater();
        });
    }

    void ApiInterface::reportClients(int workspaceId)
    {
        QNetworkRequest request((QUrl(toggl::reports::v3::CLIENTS.arg(workspaceId))));
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        if (!useSession()) {
            request
                .setRawHeader("Authorization",
                              "Basic "
                                  + QString("%1:api_token").arg(_apiToken).toLocal8Bit().toBase64());
        }
        QJsonObject body;
        body["start"] = 0;
        QJsonDocument doc(body);
        auto networkReply = _networkAccessManager->post(request, doc.toJson());

        connect(networkReply, &QNetworkReply::finished, this, [=]() {
            if (networkReply->error() == QNetworkReply::NoError) {
                auto responseData = networkReply->readAll();
                QJsonDocument doc = QJsonDocument::fromJson(responseData);
                if (doc.isArray()) {
                    QList<toggl::Client> clients;
                    auto array = doc.array();
                    for (auto v : array) {
                        clients.append(toggl::Client(v.toObject()));
                    }
                    qInfo() << "Clients" << clients.length();
                    emit reportClientsAnswer(clients);
                }
            } else {
                qWarning() << networkReply->error() << networkReply->errorString();
            }
            networkReply->deleteLater();
        });
    }

    void ApiInterface::reportProjects(int workspaceId)
    {
        QNetworkRequest request((QUrl(toggl::reports::v3::PROJECTS.arg(workspaceId))));
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        if (!useSession()) {
            request
                .setRawHeader("Authorization",
                              "Basic "
                                  + QString("%1:api_token").arg(_apiToken).toLocal8Bit().toBase64());
        }
        QJsonObject body;
        body["start"] = 0;
        body["is_active"] = true;
        QJsonDocument doc(body);
        auto networkReply = _networkAccessManager->post(request, doc.toJson());
        connect(networkReply, &QNetworkReply::finished, this, [=]() {
            if (networkReply->error() == QNetworkReply::NoError) {
                auto responseData = networkReply->readAll();
                QJsonDocument doc = QJsonDocument::fromJson(responseData);
                if (doc.isArray()) {
                    QList<toggl::Project> projects;
                    auto array = doc.array();
                    for (auto v : array) {
                        projects.append(toggl::Project(v.toObject()));
                    }
                    qInfo() << "Projects" << projects.length();
                    emit reportProjectsAnswer(projects);
                }
            } else {
                qWarning() << networkReply->error() << networkReply->errorString();
            }
            networkReply->deleteLater();
        });
    }

    void ApiInterface::reportTasks(int workspaceId)
    {
        QNetworkRequest request((QUrl(toggl::reports::v3::TASKS.arg(workspaceId))));
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        if (!useSession()) {
            request
                .setRawHeader("Authorization",
                              "Basic "
                                  + QString("%1:api_token").arg(_apiToken).toLocal8Bit().toBase64());
        }
        QJsonObject body;
        body["start"] = 0;
        body["project_active"] = true;
        body["page_size"] = 2000;
        QJsonDocument doc(body);
        auto networkReply = _networkAccessManager->post(request, doc.toJson());
        connect(networkReply, &QNetworkReply::finished, this, [=]() {
            if (networkReply->error() == QNetworkReply::NoError) {
                auto responseData = networkReply->readAll();
                QJsonDocument doc = QJsonDocument::fromJson(responseData);
                if (doc.isArray()) {
                    QList<toggl::Task> tasks;
                    auto array = doc.array();
                    for (auto v : array) {
                        tasks.append(toggl::Task(v.toObject()));
                    }
                    qInfo() << "Tasks" << tasks.length();
                    emit reportTasksAnswer(tasks);
                }
            } else {
                qWarning() << networkReply->error() << networkReply->errorString();
            }
            networkReply->deleteLater();
        });
    }

    void ApiInterface::details(int workspaceId,
                               const QDate &startDate,
                               const QDate &endDate,
                               const QList<ReportUser> &users,
                               const QList<Client> &clients,
                               const QList<Project> &projects,
                               const QList<toggl::Task> &tasks)
    {
        QNetworkRequest request((QUrl(toggl::reports::v3::DETAILS_CSV.arg(workspaceId))));
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        if (!useSession()) {
            request
                .setRawHeader("Authorization",
                              "Basic "
                                  + QString("%1:api_token").arg(_apiToken).toLocal8Bit().toBase64());
        }
        QJsonObject body;
        body["start_date"] = startDate.toString("yyyy-MM-dd");
        body["end_date"] = endDate.toString("yyyy-MM-dd");
        QJsonDocument doc(body);
        auto networkReply = _networkAccessManager->post(request, doc.toJson());
        connect(networkReply, &QNetworkReply::finished, this, [=]() {
            if (networkReply->error() == QNetworkReply::NoError) {
                TimeEntryList timeEntries;
                auto responseData = networkReply->readAll();
                const auto dataList = responseData.split(QChar::LineFeed);
                if (!dataList.isEmpty()) {
                    const auto header = dataList.first().trimmed().toLower().replace(' ', '_').split(
                        ',');
                    bool skip = true;
                    for (const auto &row : dataList) {
                        if (skip) {
                            skip = false;
                            continue;
                        }
                        const auto rowData = row.trimmed().split(',');
                        QJsonObject obj;
                        QTime startTime, endTime;
                        QDate startDate, endDate;
                        QByteArray project, task;
                        int clientId = -1, projectId = -1;
                        for (int i = 0; i < qMin(rowData.length(), header.length()); ++i) {
                            const auto headerData = header[i];
                            const auto data = rowData[i];
                            if (headerData == "user") {
                                continue;
                            } else if (headerData == "email") {
                                auto it = std::find_if(std::begin(users),
                                                       std::end(users),
                                                       [=](const toggl::ReportUser &user) {
                                                           return user.get<QString>(
                                                                          toggl::ReportUser::EMAIL)
                                                                      .value_or("")
                                                                      .toLower()
                                                                  == QString::fromUtf8(data)
                                                                         .toLower();
                                                       });
                                if (it != std::end(users)) {
                                    obj[TimeEntry::USER_ID]
                                        = (*it).get<int>(toggl::ReportUser::ID).value_or(-1);
                                }
                            } else if (headerData == "client") {
                                auto it
                                    = std::find_if(std::begin(clients),
                                                   std::end(clients),
                                                   [=](const toggl::Client &client) {
                                                       return client
                                                                  .get<QString>(toggl::Client::NAME)
                                                                  .value_or("")
                                                                  .toLower()
                                                              == QString::fromUtf8(data).toLower();
                                                   });
                                if (it != std::end(clients)) {
                                    clientId = (*it).get<int>(toggl::Client::ID).value_or(-1);
                                }
                            } else if (headerData == "project") {
                                if (clientId == -1) {
                                    project = data;
                                } else {
                                    auto it = std::find_if(
                                        std::begin(projects),
                                        std::end(projects),
                                        [=](const toggl::Project &project) {
                                            return project.get<QString>(toggl::Project::NAME)
                                                           .value_or("")
                                                           .toLower()
                                                       == QString::fromUtf8(data).toLower()
                                                   && project.get<int>(toggl::Project::CLIENT_ID)
                                                              .value_or(-1)
                                                          == clientId;
                                        });
                                    if (it != std::end(projects)) {
                                        projectId = (*it).get<int>(toggl::Project::ID).value_or(-1);
                                        obj[TimeEntry::PROJECT_ID] = QJsonValue(projectId);
                                    }
                                }
                            } else if (headerData == "task") {
                                if (projectId == -1) {
                                    task = data;
                                } else {
                                    auto it = std::find_if(
                                        std::begin(tasks),
                                        std::end(tasks),
                                        [=](const toggl::Task &task) {
                                            return task.get<QString>(toggl::Task::NAME)
                                                           .value_or("")
                                                           .toLower()
                                                       == QString::fromUtf8(data).toLower()
                                                   && task.get<int>(toggl::Task::PROJECT_ID)
                                                              .value_or(-1)
                                                          == projectId;
                                        });
                                    if (it != std::end(tasks)) {
                                        obj[TimeEntry::TASK_ID] = QJsonValue(
                                            (*it).get<int>(toggl::Task::ID).value_or(-1));
                                    }
                                }
                            } else if (headerData == "description") {
                                obj[TimeEntry::DESCRIPTION] = QJsonValue(QString::fromUtf8(data));
                            } else if (headerData == "billable") {
                                obj[TimeEntry::BILLABLE] = QJsonValue(data == "Yes");
                            } else if (headerData == "start_date") {
                                startDate = QDate::fromString(data, "yyyy-MM-dd");
                                if (startDate.isValid() && startTime.isValid()) {
                                    obj[TimeEntry::START] = QJsonValue(
                                        QDateTime(startDate, startTime).toString(Qt::ISODate));
                                }
                            } else if (headerData == "start_time") {
                                startTime = QTime::fromString(data, "HH:mm:ss");
                                if (startDate.isValid() && startTime.isValid()) {
                                    obj[TimeEntry::START] = QJsonValue(
                                        QDateTime(startDate, startTime).toString(Qt::ISODate));
                                }
                            } else if (headerData == "end_date") {
                                endDate = QDate::fromString(data, "yyyy-MM-dd");
                                if (endDate.isValid() && endTime.isValid()) {
                                    obj[TimeEntry::STOP] = QJsonValue(
                                        QDateTime(endDate, endTime).toString(Qt::ISODate));
                                }
                            } else if (headerData == "end_time") {
                                endTime = QTime::fromString(data, "HH:mm:ss");
                                if (endDate.isValid() && endTime.isValid()) {
                                    obj[TimeEntry::STOP] = QJsonValue(
                                        QDateTime(endDate, endTime).toString(Qt::ISODate));
                                }
                            } else if (headerData == "duration") {
                                bool ok = true;
                                const double value = data.toDouble(&ok);
                                if (ok) {
                                    obj[TimeEntry::DURATION] = QJsonValue(qint64(value * 3600));
                                } else {
                                    const auto durationSplit = data.split(':');
                                    if (durationSplit.length() == 3) {
                                        obj[TimeEntry::DURATION] = durationSplit[0].toInt() * 3600
                                                                   + durationSplit[1].toInt() * 60
                                                                   + durationSplit[2].toInt();
                                    } else {
                                        obj[TimeEntry::DURATION] = 0;
                                    }
                                }
                            } else if (headerData == "tags") {
                                continue;
                            } else if (headerData == "currency") {
                                continue;
                            } else if (headerData == "amount") {
                                continue;
                            }
                        }

                        if (!project.isEmpty() && clientId != -1) {
                            auto it = std::find_if(
                                std::begin(projects),
                                std::end(projects),
                                [=](const toggl::Project &_project) {
                                    return _project.get<QString>(toggl::Project::NAME)
                                                   .value_or("")
                                                   .toLower()
                                               == QString::fromUtf8(project).toLower()
                                           && _project.get<int>(toggl::Project::CLIENT_ID)
                                                      .value_or(-1)
                                                  == clientId;
                                });
                            if (it != std::end(projects)) {
                                projectId = (*it).get<int>(toggl::Project::ID).value_or(-1);
                                obj[TimeEntry::PROJECT_ID] = QJsonValue(projectId);
                            }
                        }
                        if (!task.isEmpty() && projectId != -1) {
                            auto it = std::find_if(
                                std::begin(tasks), std::end(tasks), [=](const toggl::Task &_task) {
                                    return _task.get<QString>(toggl::Task::NAME)
                                                   .value_or("")
                                                   .toLower()
                                               == QString::fromUtf8(task).toLower()
                                           && _task.get<int>(toggl::Task::PROJECT_ID).value_or(-1)
                                                  == projectId;
                                });
                            if (it != std::end(tasks)) {
                                obj[TimeEntry::TASK_ID] = QJsonValue(
                                    (*it).get<int>(toggl::Task::ID).value_or(-1));
                            }
                        }
                        timeEntries.append(toggl::TimeEntry(obj));
                    }

                } else {
                    qWarning() << "Empty detail list";
                }
                qInfo() << "Time entries" << timeEntries.length();
                emit detailsAnswer(timeEntries);
            } else {
                qWarning() << networkReply->error() << networkReply->errorString();
            }
            networkReply->deleteLater();
        });
    }

} // namespace toggl
