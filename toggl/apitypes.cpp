#include "apitypes.hpp"

#include <QJsonArray>
#include <QJsonValue>

namespace toggl {

    template<>
    std::optional<bool> ApiDataType::get<bool>(const QString &fieldName) const
    {
        if (const QJsonValue v = data[fieldName]; v.isBool()) {
            return v.toBool();
        }
        return std::nullopt;
    }
    template<>
    std::optional<QString> ApiDataType::get<QString>(const QString &fieldName) const
    {
        if (const QJsonValue v = data[fieldName]; v.isString()) {
            return v.toString();
        }
        return std::nullopt;
    }
    template<>
    std::optional<int> ApiDataType::get<int>(const QString &fieldName) const
    {
        if (const QJsonValue v = data[fieldName]; v.isDouble()) {
            return v.toInt();
        }
        return std::nullopt;
    }

    template<>
    std::optional<qint64> ApiDataType::get<qint64>(const QString &fieldName) const
    {
        if (const QJsonValue v = data[fieldName]; v.isDouble()) {
            return v.toInteger();
        }
        return std::nullopt;
    }

    template<>
    std::optional<double> ApiDataType::get<double>(const QString &fieldName) const
    {
        if (const QJsonValue v = data[fieldName]; v.isDouble()) {
            return v.toDouble();
        }
        return std::nullopt;
    }
    template<>
    std::optional<QDateTime> ApiDataType::get<QDateTime>(const QString &fieldName) const
    {
        if (const QJsonValue v = data[fieldName]; v.isString()) {
            return QDateTime::fromString(v.toString(), Qt::ISODate);
        }
        return std::nullopt;
    }
    template<>
    std::optional<QList<int>> ApiDataType::get<QList<int>>(const QString &fieldName) const
    {
        if (const QJsonValue v = data[fieldName]; v.isArray()) {
            const auto arr = v.toArray();
            QList<int> r;
            std::transform(std::begin(arr),
                           std::end(arr),
                           std::back_inserter(r),
                           [](const QJsonValue &value) { return value.toInt(); });
            return r;
        }

        return std::nullopt;
    }
    template<>
    std::optional<QList<double>> ApiDataType::get<QList<double>>(const QString &fieldName) const
    {
        if (const QJsonValue v = data[fieldName]; v.isArray()) {
            const auto arr = v.toArray();
            QList<double> r;
            std::transform(std::begin(arr),
                           std::end(arr),
                           std::back_inserter(r),
                           [](const QJsonValue &value) { return value.toDouble(); });
            return r;
        }
        return std::nullopt;
    }
    template<>
    std::optional<QList<QString>> ApiDataType::get<QList<QString>>(const QString &fieldName) const
    {
        if (const QJsonValue v = data[fieldName]; v.isArray()) {
            const auto arr = v.toArray();
            QList<QString> r;
            std::transform(std::begin(arr),
                           std::end(arr),
                           std::back_inserter(r),
                           [](const QJsonValue &value) { return value.toString(); });
            return r;
        }
        return std::nullopt;
    }

    const QString Me::API_TOKEN = "api_token";
    const QString Me::EMAIL = "email";
    const QString Me::FULLNAME = "fullname";
    const QString Me::ID = "id";
    const QString Me::IMAGE_URL = "image_url";
    const QString Me::DEFAULT_WORKSPACE_ID = "default_workspace_id";

    const QString Workspace::ID = "id";
    const QString Workspace::ORGANIZATION_ID = "organization_id";
    const QString Workspace::NAME = "name";
    const QString Workspace::PREMIUM = "premium";
    const QString Workspace::ADMIN = "admin";
    const QString Workspace::DEFAULT_CURRENCY = "default_currency";
    const QString Workspace::ROUNDING = "rounding";
    const QString Workspace::ROUNDING_MINUTES = "rounding_minutes";
    const QString Workspace::LOGO_URL = "logo_url";

    const QString TimeEntry::ID = "id";
    const QString TimeEntry::BILLABLE = "billable";
    const QString TimeEntry::DESCRIPTION = "description";
    const QString TimeEntry::DURATION = "duration";
    const QString TimeEntry::PROJECT_ID = "project_id";
    const QString TimeEntry::SERVER_DELETED_AT = "server_deleted_at";
    const QString TimeEntry::START = "start";
    const QString TimeEntry::STOP = "stop";
    const QString TimeEntry::TAG_IDS = "tag_ids";
    const QString TimeEntry::TAGS = "tags";
    const QString TimeEntry::TASK_ID = "task_id";
    const QString TimeEntry::USER_ID = "user_id";
    const QString TimeEntry::WORKSPACE_ID = "workspace_id";

    const QString ReportUser::ID = "id";
    const QString ReportUser::EMAIL = "email";
    const QString ReportUser::FULLNAME = "fullname";
    const QString ReportUser::AVATAR = "avatar";
    const QString ReportUser::ADMIN = "admin";
    const QString ReportUser::ACTIVE = "active";
    const QString ReportUser::DEACTIVATED = "deactivated";

    const QString Client::ID = "id";
    const QString Client::NAME = "name";

    const QString Project::ID = "id";
    const QString Project::WORKSPACE_ID = "workspace_id";
    const QString Project::CLIENT_ID = "client_id";
    const QString Project::NAME = "name";
    const QString Project::IS_PRIVATE = "is_private";
    const QString Project::COLOR = "color";
    const QString Project::BILLABLE = "billable";

    const QString Task::ID = "id";
    const QString Task::WORKSPACE_ID = "workspace_id";
    const QString Task::PROJECT_ID = "project_id";
    const QString Task::NAME = "name";
    const QString Task::ACTIVE = "active";

} // namespace toggl
