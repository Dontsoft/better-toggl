#ifndef APITYPES_HPP
#define APITYPES_HPP

#include <optional>

#include <QJsonObject>

namespace toggl {

    class ApiDataType
    {
    public:
        ApiDataType() {}
        ApiDataType(const QJsonObject &obj)
            : data{obj}
        {}

        template<typename T>
        std::optional<T> get(const QString &fieldName) const;
#if defined(__clang__)
        template<>
        std::optional<bool> get<bool>(const QString &fieldName) const;
        template<>
        std::optional<QString> get<QString>(const QString &fieldName) const;
        template<>
        std::optional<int> get<int>(const QString &fieldName) const;
        template<>
        std::optional<qint64> get<qint64>(const QString &fieldName) const;
        template<>
        std::optional<double> get<double>(const QString &fieldName) const;
        template<>
        std::optional<QDateTime> get<QDateTime>(const QString &fieldName) const;
        template<>
        std::optional<QList<int>> get<QList<int>>(const QString &fieldName) const;
        template<>
        std::optional<QList<double>> get<QList<double>>(const QString &fieldName) const;
        template<>
        std::optional<QList<QString>> get<QList<QString>>(const QString &fieldName) const;
#endif

        void set(const QString &fieldName);
        QJsonObject getData() const { return data; }

    private:
        QJsonObject data;
    };

    class Me : public ApiDataType
    {
    public:
        using ApiDataType::ApiDataType;

        static const QString API_TOKEN;
        static const QString EMAIL;
        static const QString FULLNAME;
        static const QString ID;
        static const QString IMAGE_URL;
        static const QString DEFAULT_WORKSPACE_ID;
    };

    class Workspace : public ApiDataType
    {
    public:
        using ApiDataType::ApiDataType;

        static const QString ID;
        static const QString ORGANIZATION_ID;
        static const QString NAME;
        static const QString PREMIUM;
        static const QString ADMIN;
        static const QString DEFAULT_CURRENCY;
        static const QString ROUNDING;
        static const QString ROUNDING_MINUTES;
        static const QString LOGO_URL;
    };

    class TimeEntry : public ApiDataType
    {
    public:
        using ApiDataType::ApiDataType;

        static const QString ID;
        static const QString BILLABLE;
        static const QString DESCRIPTION;
        static const QString DURATION;
        static const QString PROJECT_ID;
        static const QString SERVER_DELETED_AT;
        static const QString START;
        static const QString STOP;
        static const QString TAG_IDS;
        static const QString TAGS;
        static const QString TASK_ID;
        static const QString USER_ID;
        static const QString WORKSPACE_ID;
    };

    class ReportUser : public ApiDataType
    {
    public:
        using ApiDataType::ApiDataType;

        static const QString ID;
        static const QString EMAIL;
        static const QString FULLNAME;
        static const QString AVATAR;
        static const QString ADMIN;
        static const QString ACTIVE;
        static const QString DEACTIVATED;
    };

    class Client : public ApiDataType
    {
    public:
        using ApiDataType::ApiDataType;

        static const QString ID;
        static const QString NAME;
    };

    class Project : public ApiDataType
    {
    public:
        using ApiDataType::ApiDataType;

        static const QString ID;
        static const QString WORKSPACE_ID;
        static const QString CLIENT_ID;
        static const QString NAME;
        static const QString IS_PRIVATE;
        static const QString COLOR;
        static const QString BILLABLE;
    };

    class Task : public ApiDataType
    {
    public:
        using ApiDataType::ApiDataType;

        /*
    {
        "id": 92682124,
        "name": "Workday - Senior Consultant",
        "workspace_id": 1826532,
        "project_id": 193140290,
        "user_id": null,
        "recurring": false,
        "active": true,
        "at": "2023-06-26T11:56:28+00:00",
        "server_deleted_at": null,
        "estimated_seconds": 864000,
        "tracked_seconds": 567071
    }
        */

        static const QString ID;
        static const QString WORKSPACE_ID;
        static const QString PROJECT_ID;
        static const QString NAME;
        static const QString ACTIVE;
    };

#if !defined(__clang__)
    template<>
    std::optional<bool> ApiDataType::get<bool>(const QString &fieldName) const;
    template<>
    std::optional<QString> ApiDataType::get<QString>(const QString &fieldName) const;
    template<>
    std::optional<int> ApiDataType::get<int>(const QString &fieldName) const;
    template<>
    std::optional<qint64> ApiDataType::get<qint64>(const QString &fieldName) const;
    template<>
    std::optional<double> ApiDataType::get<double>(const QString &fieldName) const;
    template<>
    std::optional<QDateTime> ApiDataType::get<QDateTime>(const QString &fieldName) const;
    template<>
    std::optional<QList<int>> ApiDataType::get<QList<int>>(const QString &fieldName) const;
    template<>
    std::optional<QList<double>> ApiDataType::get<QList<double>>(const QString &fieldName) const;
    template<>
    std::optional<QList<QString>> ApiDataType::get<QList<QString>>(const QString &fieldName) const;
#endif

} // namespace toggl

Q_DECLARE_METATYPE(toggl::Me);
Q_DECLARE_METATYPE(toggl::Workspace);
Q_DECLARE_METATYPE(toggl::TimeEntry);
Q_DECLARE_METATYPE(toggl::ReportUser);
Q_DECLARE_METATYPE(toggl::Client);
Q_DECLARE_METATYPE(toggl::Project);
Q_DECLARE_METATYPE(toggl::Task);

#endif // APITYPES_HPP
