#ifndef LOGOUTDIALOG_HPP
#define LOGOUTDIALOG_HPP

#include <QDialog>

namespace Ui {
    class LogoutDialog;
}

class LogoutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LogoutDialog(QWidget *parent = nullptr);
    ~LogoutDialog();

private:
    Ui::LogoutDialog *ui;
};

#endif // LOGOUTDIALOG_HPP
