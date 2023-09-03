#ifndef BUSYINDICATORDIALOG_HPP
#define BUSYINDICATORDIALOG_HPP

#include <QDialog>

namespace Ui {
    class BusyIndicatorDialog;
}

class BusyIndicatorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BusyIndicatorDialog(QWidget *parent = nullptr);
    ~BusyIndicatorDialog();

public slots:
    void setIndicatorMessage(const QString &message);

private:
    Ui::BusyIndicatorDialog *ui;
};

#endif // BUSYINDICATORDIALOG_HPP
