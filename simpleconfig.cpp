#include "simpleconfig.hpp"

#include <QCryptographicHash>
#include <QDebug>
#include <QDir>
#include <QJsonDocument>
#include <QSettings>
#include <QStandardPaths>

#include "qaesencryption.h"

const QString SimpleConfig::UI_THEME = QStringLiteral("Display/Theme");
const QString SimpleConfig::DISPLAY_BILLABLE_ONLY = QStringLiteral("Display/BillableOnly");
const QString SimpleConfig::DISPLAY_ROUNDED = QStringLiteral("Display/Rounded");
const QString SimpleConfig::WORKSPACE = QStringLiteral("Workspace");
const QString SimpleConfig::USE_WORKSPACE_ROUNDING = QStringLiteral("Display/UseWorkspaceRounding");
const QString SimpleConfig::CUSTOM_ROUNDING = QStringLiteral("Display/CustomRounding");
const QString SimpleConfig::CUSTOM_ROUNDING_MINUTES = QStringLiteral(
    "Display/CustomRoundingMinutes");
const QString SimpleConfig::API_TOKEN = QStringLiteral("Admin/ApiTokenEncrypted");
const QString SimpleConfig::BILLING_HIDE_EMPTY_PROJECTS = QStringLiteral(
    "Billing/HideEmptyProjects");
const QString SimpleConfig::EXPORT_DEFAULT_LOCATION = QStringLiteral("Export/DefaultLocation");
const QString SimpleConfig::EXPORT_TITLE = QStringLiteral("Export/Title");
const QString SimpleConfig::EXPORT_SUBJECT = QStringLiteral("Export/Subject");
const QString SimpleConfig::EXPORT_CREATOR = QStringLiteral("Export/Creator");
const QString SimpleConfig::EXPORT_COMPANY = QStringLiteral("Export/Company");
const QString SimpleConfig::EXPORT_DESCRIPTION = QStringLiteral("Export/Description");

static const QByteArray ENCRYPTION_KEY = QCryptographicHash::hash(QSysInfo::machineUniqueId(),
                                                                  QCryptographicHash::Blake2s_256);

SimpleConfig::SimpleConfig(QObject* parent)
    : QObject{parent}
    , _settings(new QSettings(getPath(), QSettings::IniFormat))
{
    qInfo() << ENCRYPTION_KEY;
}

void SimpleConfig::set(const QString& key, const QVariant& value)
{
    _settings->setValue(key, value);
    emit changed(key);
}

QVariant SimpleConfig::get(const QString& key, const QVariant& defaultValue) const
{
    return _settings->value(key, defaultValue);
}

void SimpleConfig::clearAuthenticatedData()
{
    _settings->remove(WORKSPACE);
    _settings->remove(API_TOKEN);
}

void SimpleConfig::setEncrypted(const QString& key, const QByteArray& value)
{
    _settings->setValue(key,
                        QAESEncryption::Crypt(QAESEncryption::AES_256,
                                              QAESEncryption::ECB,
                                              value,
                                              ENCRYPTION_KEY));
}

QByteArray SimpleConfig::getEncrypted(const QString& key, const QByteArray& defaultValue) const
{
    if (!_settings->contains(key)) {
        return defaultValue;
    }
    const auto value = _settings->value(key).toByteArray();
    return QAESEncryption::Decrypt(QAESEncryption::AES_256,
                                   QAESEncryption::ECB,
                                   value,
                                   ENCRYPTION_KEY);
}

QString SimpleConfig::getPath()
{
    static const auto basePath = QStandardPaths::writableLocation(
                                     QStandardPaths::AppLocalDataLocation)
                                 + "/";
    if (!QDir(basePath).exists()) {
        QDir().mkpath(basePath);
    }
    return QString("%1%2").arg(QDir().toNativeSeparators(basePath), "config.ini");
}
