#ifndef SIMPLECONFIG_HPP
#define SIMPLECONFIG_HPP

#include <QJsonObject>
#include <QObject>
#include <QScopedPointer>
#include <QSettings>

class QSettings;

class SimpleConfig : public QObject
{
    Q_OBJECT
public:
    enum Theme { LightTheme = 0, DarkTheme = 1 };

    Q_ENUM(Theme)

    static const QString UI_THEME;

    static const QString DISPLAY_BILLABLE_ONLY;
    static const QString DISPLAY_ROUNDED;
    static const QString WORKSPACE;
    static const QString USE_WORKSPACE_ROUNDING;
    static const QString CUSTOM_ROUNDING;
    static const QString CUSTOM_ROUNDING_MINUTES;
    static const QString API_TOKEN;
    static const QString BILLING_HIDE_EMPTY_PROJECTS;
    static const QString EXPORT_DEFAULT_LOCATION;
    static const QString EXPORT_TITLE;
    static const QString EXPORT_SUBJECT;
    static const QString EXPORT_CREATOR;
    static const QString EXPORT_COMPANY;
    static const QString EXPORT_DESCRIPTION;

    explicit SimpleConfig(QObject* parent = nullptr);
    void set(const QString& key, const QVariant& value);
    QVariant get(const QString& key, const QVariant& defaultValue = QVariant()) const;

    void setEncrypted(const QString& key, const QByteArray& value);
    QByteArray getEncrypted(const QString& key, const QByteArray& defaultValue = QByteArray()) const;

    void clearAuthenticatedData();
signals:
    void changed(const QString& key);

private:
    static QString getPath();
    QScopedPointer<QSettings> _settings;
};

#endif // SIMPLECONFIG_HPP
