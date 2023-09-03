#ifndef STATUSBAR_HPP
#define STATUSBAR_HPP

#include <QSharedPointer>
#include <QWidget>

class GlobalStorage;

namespace Ui {
    class StatusBar;
}

class StatusBar : public QWidget
{
    Q_OBJECT

public:
    explicit StatusBar(QWidget *parent = nullptr);
    ~StatusBar();

public slots:
    void updateWorkspace(const QString &workspaceName, const QString &workspaceImageUrl);
    void updateUser(const QString &name, const QString &userImageUrl);

    void setGlobalStorageToImageViewer(QSharedPointer<GlobalStorage> globalStorage);

private:
    Ui::StatusBar *ui;
};

#endif // STATUSBAR_HPP
