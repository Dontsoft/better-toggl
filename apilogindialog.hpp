#ifndef APILOGINDIALOG_HPP
#define APILOGINDIALOG_HPP

#include <QDialog>
#include <QSharedPointer>

class QAbstractButton;
class SimpleConfig;

#include "toggl/apiinterface.hpp"

namespace Ui {
    class ApiLoginDialog;
}

class ApiLoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ApiLoginDialog(QSharedPointer<toggl::ApiInterface> apiInterface,
                            QSharedPointer<SimpleConfig> simpleConfig,
                            QWidget *parent = nullptr);
    ~ApiLoginDialog();

private slots:
    void buttonClicked(QAbstractButton *button);
    void loginClicked();

    void authAnswer(bool success, const QString &errorText = QString());

private:
    Ui::ApiLoginDialog *ui;
    QSharedPointer<toggl::ApiInterface> _apiInterface;
    QSharedPointer<SimpleConfig> _simpleConfig;
};

#endif // APILOGINDIALOG_HPP
