#include "persistentstorage.hpp"

#include <QDebug>
#include <QDir>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>

PersistentStorage::PersistentStorage(QObject* parent)
    : QObject{parent}
{
    initDatabase();
}

void PersistentStorage::initDatabase() const
{
    qDebug() << __PRETTY_FUNCTION__;
    auto db = getDatabase();
    if (db.open() && db.isOpen()) {
        const auto tables = db.tables();
        if (!tables.contains("toggl_cookies")) {
            auto query = db.exec(
                "CREATE TABLE toggl_cookies (int INTEGER PRIMARY KEY AUTOINCREMENT, name "
                "TEXT, value TEXT);");
            if (query.lastError().isValid()) {
                qWarning() << query.lastError().databaseText();
            }
        }
    } else {
        qWarning() << "Database not open";
    }
}

QString PersistentStorage::getCookie(const QString& cookie) const
{
    qDebug() << __PRETTY_FUNCTION__;
    auto db = getDatabase();
    if (db.open() && db.isOpen()) {
        QSqlQuery query(db);
        query.prepare("SELECT value from toggl_cookies WHERE name = ?");
        query.bindValue(0, cookie);
        if (query.exec()) {
            if (query.first()) {
                const auto value = query.value("value").toString();
                query.finish();
                return value;
            }
            query.finish();
        } else {
            qWarning() << query.lastError().databaseText();
        }
    } else {
        qWarning() << "Database not open";
    }
    return QString();
}

void PersistentStorage::setCookie(const QString& cookie, const QString& value) const
{
    qDebug() << __PRETTY_FUNCTION__;
    auto db = getDatabase();
    if (db.isOpen()) {
        QSqlQuery query(db);
        query.prepare("SELECT value from toggl_cookies WHERE name = ?");
        query.bindValue(0, cookie);
        if (query.exec()) {
            if (query.first()) {
                const auto value = query.value("value").toString();
                qInfo() << "Updating current value" << value;
                query.finish();
                query.prepare("UPDATE toggl_cookies SET value = ? WHERE name = ?");
                query.bindValue(0, value);
                query.bindValue(1, cookie);
                if (!query.exec()) {
                    qWarning() << query.lastError().databaseText();
                }
            } else {
                query.finish();
                query.prepare("INSERT INTO toggl_cookies (value, name) VALUES (?, ?)");
                query.bindValue(0, value);
                query.bindValue(1, cookie);
                if (!query.exec()) {
                    qWarning() << query.lastError().databaseText();
                }
            }
        } else {
            qWarning() << query.lastError().databaseText();
        }
    } else {
        qWarning() << "Database not open";
    }
}

void PersistentStorage::deleteCookie(const QString& cookie) const
{
    qDebug() << __PRETTY_FUNCTION__;
    auto db = getDatabase();
    if (db.open() && db.isOpen()) {
        QSqlQuery query(db);
        query.prepare("DELETE from toggl_cookies WHERE name = ?");
        query.bindValue(0, cookie);
        if (!query.exec()) {
            qWarning() << query.lastError().databaseText();
        }
    } else {
        qWarning() << "Database not open";
    }
}

QString PersistentStorage::getDatabasePath()
{
    qDebug() << __PRETTY_FUNCTION__;
    static const auto basePath = QStandardPaths::writableLocation(
                                     QStandardPaths::AppLocalDataLocation)
                                 + "/";
    if (!QDir(basePath).exists()) {
        QDir().mkpath(basePath);
    }
    return QString("%1%2").arg(QDir().toNativeSeparators(basePath), "cookies.sqlite");
}

QSqlDatabase PersistentStorage::getDatabase() const
{
    qDebug() << __PRETTY_FUNCTION__;
    const auto path = PersistentStorage::getDatabasePath();
    {
        QFile f(path);
        if (!f.exists()) {
            if (!f.open(QIODevice::WriteOnly)) {
                qWarning() << "Could not open file";
            }
        }
    }
    if (!QSqlDatabase::contains("persistent_storage")) {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "persistent_storage");
        db.setDatabaseName(path);
        if (!db.open()) {
            qWarning() << db.lastError().databaseText();
        }
    }

    auto db = QSqlDatabase::database("persistent_storage", true);
    if (!db.open()) {
        qWarning() << "Not able to open";
    }
    return db;
}
