#ifndef APIENDPOINTS_HPP
#define APIENDPOINTS_HPP

#include <QString>
#include <QStringList>
#include <QStringView>

namespace toggl {
    static const QString API_BASEURL = QStringLiteral("https://api.track.toggl.com");
    namespace v9 {

        static const QString BASEURL = API_BASEURL + QStringLiteral("/api/v9/");
        static const QString build_endpoint(const QStringList& endpointPath)
        {
            return BASEURL + endpointPath.join("/");
        }
        static const QString ME = build_endpoint({"me"});
        static const QString SESSIONS = build_endpoint({"me", "sessions"});
        static const QString WORKSPACES = build_endpoint({"me", "workspaces"});
        static const QString TIME_ENTRIES = build_endpoint({"me", "time_entries"});

    } // namespace v9
    namespace reports {
        namespace v3 {
            static const QString BASEURL = API_BASEURL + QStringLiteral("/reports/api/v3/");

            static const QString build_endpoint(const QStringList& endpointPath)
            {
                return BASEURL + endpointPath.join("/");
            }
            static const QString DETAILS_CSV = build_endpoint(
                {"workspace", "%1", "search", "time_entries.csv"});
            static const QString USERS = build_endpoint({"workspace", "%1", "search", "users"});
            static const QString CLIENTS = build_endpoint({"workspace", "%1", "filters", "clients"});
            static const QString PROJECTS = build_endpoint(
                {"workspace", "%1", "search", "projects"});
            static const QString TASKS = build_endpoint({"workspace", "%1", "search", "tasks"});
        } // namespace v3

    } // namespace reports
} // namespace toggl

#endif // APIENDPOINTS_HPP
