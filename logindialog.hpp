#ifndef LOGINDIALOG_HPP
#define LOGINDIALOG_HPP

#include <QDialog>
#include <QSharedPointer>

class QAbstractButton;
class PersistentStorage;

#include "toggl/apiinterface.hpp"

namespace Ui {
    class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QSharedPointer<toggl::ApiInterface> apiInterface,
                         QSharedPointer<PersistentStorage> persistentStorage,
                         QWidget *parent = nullptr);
    ~LoginDialog();

private slots:
    void buttonClicked(QAbstractButton *button);
    void loginClicked();

    void authAnswer(bool success, const QString &errorText = QString());

private:
    Ui::LoginDialog *ui;
    QSharedPointer<toggl::ApiInterface> _apiInterface;
    QSharedPointer<PersistentStorage> _persistentStorage;
};

#endif // LOGINDIALOG_HPP
