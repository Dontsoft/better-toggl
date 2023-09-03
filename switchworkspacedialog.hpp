#ifndef SWITCHWORKSPACEDIALOG_HPP
#define SWITCHWORKSPACEDIALOG_HPP

#include <QDialog>
#include <QSharedPointer>

class GlobalStorage;
class SimpleConfig;

namespace Ui {
    class SwitchWorkspaceDialog;
}

class SwitchWorkspaceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SwitchWorkspaceDialog(QSharedPointer<GlobalStorage> globalStorage,
                                   QSharedPointer<SimpleConfig> simpleConfig,
                                   QWidget *parent = nullptr);
    ~SwitchWorkspaceDialog();

public slots:
    void switchWorkspaceButtonClicked();

private:
    Ui::SwitchWorkspaceDialog *ui;
    QSharedPointer<GlobalStorage> _globalStorage;
    QSharedPointer<SimpleConfig> _simpleConfig;
};

#endif // SWITCHWORKSPACEDIALOG_HPP
