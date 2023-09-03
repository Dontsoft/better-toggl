#ifndef PERSISTENTSTORAGE_HPP
#define PERSISTENTSTORAGE_HPP

#include <QObject>
#include <QSqlDatabase>

class PersistentStorage : public QObject
{
public:
    explicit PersistentStorage(QObject* parent = nullptr);
    QString getCookie(const QString& cookie) const;
    void setCookie(const QString& cookie, const QString& value) const;
    void deleteCookie(const QString& cookie) const;

private:
    void initDatabase() const;
    static QString getDatabasePath();

    QSqlDatabase getDatabase() const;
};

#endif // PERSISTENTSTORAGE_HPP
