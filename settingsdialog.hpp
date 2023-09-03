#ifndef SETTINGSDIALOG_HPP
#define SETTINGSDIALOG_HPP

#include <QDialog>
#include <QSharedPointer>

class SimpleConfig;

namespace Ui {
    class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QSharedPointer<SimpleConfig> simpleConfig, QWidget *parent = nullptr);
    ~SettingsDialog();

    void useWorkspaceRoundingCheckStateChanged(int);
    void roundingChanged(int index);
    void roundingMinutesChanged(int index);

    void selectDefaultLocationClicked();
    void defaultLocationChanged(const QString &defaultLocation);
    void titleChanged(const QString &title);
    void subjectChanged(const QString &subject);
    void creatorChanged(const QString &creator);
    void companyChanged(const QString &company);
    void descriptionChanged(const QString &description);

private:
    Ui::SettingsDialog *ui;
    QSharedPointer<SimpleConfig> _simpleConfig;
};

#endif // SETTINGSDIALOG_HPP
