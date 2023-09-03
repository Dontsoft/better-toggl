#ifndef BETTERTOGGLMAINWINDOW_HPP
#define BETTERTOGGLMAINWINDOW_HPP

#include <tuple>

#include <QMainWindow>
#include <QSharedPointer>

#include "toggl/apiconstants.hpp"
#include "toggl/apiinterface.hpp"

class GlobalStorage;
class PersistentStorage;
class SimpleConfig;
class QLabel;
class BusyIndicatorDialog;
class SummarizedTimeEntryModel;

namespace toggl {
    class ApiInterface;
}

QT_BEGIN_NAMESPACE
namespace Ui { class BetterTogglMainWindow; }
QT_END_NAMESPACE

class QListWidgetItem;

class BetterTogglMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    BetterTogglMainWindow(QSharedPointer<toggl::ApiInterface> apiInterface,
                          QSharedPointer<toggl::ApiInterface> adminApiInterface,
                          QSharedPointer<GlobalStorage> globalStorage,
                          QSharedPointer<GlobalStorage> adminGlobalStorage,
                          QSharedPointer<PersistentStorage> persistentStorage,
                          QSharedPointer<SimpleConfig> simpleConfig,
                          QWidget *parent = nullptr);
    ~BetterTogglMainWindow();

protected:
    void resizeEvent(QResizeEvent *event);

signals:
    void updateBusyIndicatorMessage(const QString &message = QString());
    void showBusyIndicator();
    void hideBusyIndicator();

private slots:
    void globalStorageUpdated(const QString &key);
    void adminGlobalStorageUpdated(const QString &key);
    void configUpdate(const QString &key);

    void gotoTrackingPage();
    void gotoAdminPage();
    void gotoAnalyticsPage();
    void gotoBillingPage();
    void stackedWidgetIndexChanged(int index);

    void authenticationStateAnswer(bool isAuthenticated);
    void adminAuthenticationStateAnswer(bool isAuthenticated);
    void launchLoginDialog();

    void logout();
    void logoutAnswer();

    void todaySecondsChanged(qint64 totalSeconds);
    void weeklySecondsChanged(qint64 totalSeconds);
    void monthlySecondsChanged(qint64 totalSeconds);

    void detailsAnswer(const toggl::TimeEntryList &timeEntries);
    void adminDetailsAnswer(const toggl::TimeEntryList &timeEntries);

    void switchWorkspaceDialogClicked();
    void showOnlyRounded(bool checked);
    void showOnlyBillable(bool checked);

    void refreshData();
    void refreshMe();
    void refreshWorkspaces();
    void refreshClients();
    void refreshProjects();
    void refreshTasks();
    void refreshLatestTimeEntries();

    void refreshBillingData();
    void refreshBillingUsers();
    void refreshBillingClients();
    void refreshBillingProjects();
    void refreshBillingTasks();
    void refreshBillingTimeEntries(bool hideBusyIndicatorAfterExecution = true);

    void workspaceUpdated();
    void refreshMonthlyTimeDisplay();
    void refreshWeeklyTimeDisplay();
    void refreshTodayTimeDisplay();

    void refreshSummarizers();
    void storeSummarizers();
    void refreshSummarizerDisplay();

    void summarizerEntryDoubleClicked(QListWidgetItem *item);

    void hideEmptyProjectsToggled(int state);

    void billingPreviousMonth();
    void billingNextMonth();

    void summaryTreeViewEntryDoubleClicked(const QModelIndex &index);

    void settingsClicked();

    void exportBillingData();

private:
    std::tuple<toggl::Rounding, toggl::RoundingMinute, int> getRoundingConfiguration();

    void updateTimeDisplay(QLabel *displayLabel, qint64 seconds);

    Ui::BetterTogglMainWindow *ui;
    QSharedPointer<toggl::ApiInterface> _apiInterface;
    QSharedPointer<toggl::ApiInterface> _adminApiInterface;
    QSharedPointer<GlobalStorage> _globalStorage;
    QSharedPointer<GlobalStorage> _adminGlobalStorage;
    QSharedPointer<PersistentStorage> _persistentStorage;
    QSharedPointer<SimpleConfig> _simpleConfig;

    BusyIndicatorDialog *_busyIndicatorDialog;
    SummarizedTimeEntryModel *_summarizedTimeEntryModel;
};
#endif // BETTERTOGGLMAINWINDOW_HPP
