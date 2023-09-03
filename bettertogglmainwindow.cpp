#include "bettertogglmainwindow.hpp"
#include "./ui_bettertogglmainwindow.h"

#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QSettings>
#include <QStandardPaths>

#include "apilogindialog.hpp"
#include "billingsummarizerdialog.hpp"
#include "busyindicatordialog.hpp"
#include "exportdialog.hpp"
#include "globalstorage.hpp"
#include "logindialog.hpp"
#include "logoutdialog.hpp"
#include "persistentstorage.hpp"
#include "settingsdialog.hpp"
#include "simpleconfig.hpp"
#include "summarizedtimeentrymodel.hpp"
#include "switchworkspacedialog.hpp"
#include "timeentryviewerdialog.hpp"
#include "toggl/apiinterface.hpp"

BetterTogglMainWindow::BetterTogglMainWindow(QSharedPointer<toggl::ApiInterface> apiInterface,
                                             QSharedPointer<toggl::ApiInterface> adminApiInterface,
                                             QSharedPointer<GlobalStorage> globalStorage,
                                             QSharedPointer<GlobalStorage> adminGlobalStorage,
                                             QSharedPointer<PersistentStorage> persistentStorage,
                                             QSharedPointer<SimpleConfig> simpleConfig,
                                             QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::BetterTogglMainWindow)
    , _apiInterface(apiInterface)
    , _adminApiInterface(adminApiInterface)
    , _globalStorage(globalStorage)
    , _adminGlobalStorage(adminGlobalStorage)
    , _persistentStorage(persistentStorage)
    , _simpleConfig(simpleConfig)
    , _busyIndicatorDialog(new BusyIndicatorDialog(this))
    , _summarizedTimeEntryModel{
          new SummarizedTimeEntryModel(_adminGlobalStorage, _globalStorage, _simpleConfig)}
{
    ui->setupUi(this);

    setWindowTitle(tr("Better Toggl"));

    _busyIndicatorDialog->setModal(true);
    ui->statusBar->setGlobalStorageToImageViewer(_globalStorage);
    ui->todayTimeLabel->setFont(QFont("Fira Mono", 48));
    ui->thisWeekTimeLabel->setFont(QFont("Fira Mono", 24));
    ui->thisMonthTimeLabel->setFont(QFont("Fira Mono", 24));

    connect(ui->actionTime_Tracking,
            &QAction::triggered,
            this,
            &BetterTogglMainWindow::gotoTrackingPage);

    connect(ui->actionAnalytics,
            &QAction::triggered,
            this,
            &BetterTogglMainWindow::gotoAnalyticsPage);

    connect(ui->actionBilling, &QAction::triggered, this, &BetterTogglMainWindow::gotoBillingPage);

    connect(ui->stackedWidget,
            &QStackedWidget::currentChanged,
            this,
            &BetterTogglMainWindow::stackedWidgetIndexChanged);

    connect(this,
            &BetterTogglMainWindow::showBusyIndicator,
            _busyIndicatorDialog,
            &BusyIndicatorDialog::show);
    connect(this,
            &BetterTogglMainWindow::hideBusyIndicator,
            _busyIndicatorDialog,
            &BusyIndicatorDialog::hide);
    connect(this,
            &BetterTogglMainWindow::updateBusyIndicatorMessage,
            _busyIndicatorDialog,
            &BusyIndicatorDialog::setIndicatorMessage);

    ui->actionBillable_only->setChecked(
        _simpleConfig->get(SimpleConfig::DISPLAY_BILLABLE_ONLY).toBool());
    ui->actionShow_Rounded->setChecked(_simpleConfig->get(SimpleConfig::DISPLAY_ROUNDED).toBool());
    ui->hideEmptyProjectsCheckBox->setChecked(
        _simpleConfig->get(SimpleConfig::BILLING_HIDE_EMPTY_PROJECTS, true).toBool());

    ui->todayTimeLabel->setMinimumWidth(ui->todayTimeLabel->fontMetrics().averageCharWidth() * 10);
    ui->thisWeekTimeLabel->setMinimumWidth(ui->thisWeekTimeLabel->fontMetrics().averageCharWidth()
                                           * 10);
    ui->thisMonthTimeLabel->setMinimumWidth(ui->thisMonthTimeLabel->fontMetrics().averageCharWidth()
                                            * 10);

    ui->stackedWidget->setCurrentIndex(0);

    ui->summarizedTreeView->setModel(_summarizedTimeEntryModel);
    connect(_summarizedTimeEntryModel,
            &SummarizedTimeEntryModel::modelReset,
            ui->summarizedTreeView,
            &QTreeView::expandAll);
    connect(_summarizedTimeEntryModel, &SummarizedTimeEntryModel::modelReset, this, [=]() {
        ui->summarizedTreeView->resizeColumnToContents(0);
        ui->summarizedTreeView->resizeColumnToContents(1);
        ui->summarizedTreeView->resizeColumnToContents(2);
    });

    connect(ui->launchLoginButton,
            &QPushButton::clicked,
            this,
            &BetterTogglMainWindow::launchLoginDialog);
    connect(ui->actionLogout, &QAction::triggered, this, &BetterTogglMainWindow::logout);

    connect(ui->actionSync, &QAction::triggered, this, &BetterTogglMainWindow::refreshData);

    connect(ui->actionSwitch_Workspace,
            &QAction::triggered,
            this,
            &BetterTogglMainWindow::switchWorkspaceDialogClicked);

    connect(ui->actionBillable_only,
            &QAction::toggled,
            this,
            &BetterTogglMainWindow::showOnlyBillable);

    connect(ui->actionShow_Rounded,
            &QAction::toggled,
            this,
            &BetterTogglMainWindow::showOnlyRounded);

    connect(_globalStorage.data(),
            &GlobalStorage::notify,
            this,
            &BetterTogglMainWindow::globalStorageUpdated);

    connect(_adminGlobalStorage.data(),
            &GlobalStorage::notify,
            this,
            &BetterTogglMainWindow::adminGlobalStorageUpdated);

    connect(_simpleConfig.data(),
            &SimpleConfig::changed,
            this,
            &BetterTogglMainWindow::configUpdate);

    connect(_apiInterface.data(),
            &toggl::ApiInterface::checkAuthenticationStateAnswer,
            this,
            &BetterTogglMainWindow::authenticationStateAnswer);

    connect(_apiInterface.data(),
            &toggl::ApiInterface::logoutAnswer,
            this,
            &BetterTogglMainWindow::logoutAnswer);

    connect(_apiInterface.data(),
            &toggl::ApiInterface::meAnswer,
            _globalStorage.data(),
            qOverload<const toggl::Me &>(&GlobalStorage::set));

    connect(_apiInterface.data(),
            &toggl::ApiInterface::workspacesAnswer,
            _globalStorage.data(),
            qOverload<const QList<toggl::Workspace> &>(&GlobalStorage::set));

    connect(_apiInterface.data(),
            &toggl::ApiInterface::timeEntriesLatestAnswer,
            _globalStorage.data(),
            qOverload<const toggl::TimeEntryList &>(&GlobalStorage::set));

    connect(_apiInterface.data(),
            &toggl::ApiInterface::reportUsersAnswer,
            _globalStorage.data(),
            qOverload<const QList<toggl::ReportUser> &>(&GlobalStorage::set));

    connect(_apiInterface.data(),
            &toggl::ApiInterface::reportClientsAnswer,
            _globalStorage.data(),
            qOverload<const QList<toggl::Client> &>(&GlobalStorage::set));

    connect(_apiInterface.data(),
            &toggl::ApiInterface::reportProjectsAnswer,
            _globalStorage.data(),
            qOverload<const QList<toggl::Project> &>(&GlobalStorage::set));

    connect(_apiInterface.data(),
            &toggl::ApiInterface::reportTasksAnswer,
            _globalStorage.data(),
            qOverload<const QList<toggl::Task> &>(&GlobalStorage::set));

    connect(_adminApiInterface.data(),
            &toggl::ApiInterface::meAnswer,
            _adminGlobalStorage.data(),
            qOverload<const toggl::Me &>(&GlobalStorage::set));

    connect(_adminApiInterface.data(),
            &toggl::ApiInterface::workspacesAnswer,
            _adminGlobalStorage.data(),
            qOverload<const QList<toggl::Workspace> &>(&GlobalStorage::set));

    connect(_adminApiInterface.data(),
            &toggl::ApiInterface::timeEntriesLatestAnswer,
            _adminGlobalStorage.data(),
            qOverload<const toggl::TimeEntryList &>(&GlobalStorage::set));

    connect(_adminApiInterface.data(),
            &toggl::ApiInterface::reportUsersAnswer,
            _adminGlobalStorage.data(),
            qOverload<const QList<toggl::ReportUser> &>(&GlobalStorage::set));

    connect(_adminApiInterface.data(),
            &toggl::ApiInterface::reportClientsAnswer,
            _adminGlobalStorage.data(),
            qOverload<const QList<toggl::Client> &>(&GlobalStorage::set));

    connect(_adminApiInterface.data(),
            &toggl::ApiInterface::reportProjectsAnswer,
            _adminGlobalStorage.data(),
            qOverload<const QList<toggl::Project> &>(&GlobalStorage::set));

    connect(_adminApiInterface.data(),
            &toggl::ApiInterface::reportTasksAnswer,
            _adminGlobalStorage.data(),
            qOverload<const QList<toggl::Task> &>(&GlobalStorage::set));

    connect(_adminApiInterface.data(),
            &toggl::ApiInterface::detailsAnswer,
            this,
            &BetterTogglMainWindow::adminDetailsAnswer);

    connect(ui->summarizerProjectListWidget,
            &QListWidget::itemDoubleClicked,
            this,
            &BetterTogglMainWindow::summarizerEntryDoubleClicked);

    connect(ui->refreshBillingDataButton,
            &QPushButton::clicked,
            this,
            &BetterTogglMainWindow::refreshBillingData);

    connect(ui->hideEmptyProjectsCheckBox,
            &QCheckBox::toggled,
            this,
            &BetterTogglMainWindow::hideEmptyProjectsToggled);
    connect(ui->nextMonthSelectionButton,
            &QPushButton::clicked,
            this,
            &BetterTogglMainWindow::billingNextMonth);

    connect(ui->previousMonthSelectionButton,
            &QPushButton::clicked,
            this,
            &BetterTogglMainWindow::billingPreviousMonth);

    connect(ui->summarizedTreeView,
            &QTreeView::doubleClicked,
            this,
            &BetterTogglMainWindow::summaryTreeViewEntryDoubleClicked);

    connect(ui->exportBillingDataButton,
            &QPushButton::clicked,
            this,
            &BetterTogglMainWindow::exportBillingData);

    connect(ui->actionSettings, &QAction::triggered, this, &BetterTogglMainWindow::settingsClicked);

    apiInterface->checkAuthenticationState();
}

BetterTogglMainWindow::~BetterTogglMainWindow()
{
    delete ui;
}

void BetterTogglMainWindow::resizeEvent(QResizeEvent *event)
{
    _simpleConfig->set(QString("UI/Display%1").arg(ui->stackedWidget->currentIndex()), geometry());
    QMainWindow::resizeEvent(event);
}

void BetterTogglMainWindow::globalStorageUpdated(const QString &key)
{
    if (key == GlobalStorage::ME) {
        auto me = _globalStorage->me();
        auto name = me.get<QString>(toggl::Me::FULLNAME);
        auto userImageUrl = me.get<QString>(toggl::Me::IMAGE_URL);
        ui->statusBar->updateUser(name.value_or(""), userImageUrl.value_or(""));
    } else if (key == GlobalStorage::WORKSPACES) {
        auto workspaces = _globalStorage->workspaces();
        ui->actionSwitch_Workspace->setVisible(workspaces.length() > 1);
        workspaceUpdated();
    } else if (key == GlobalStorage::LATEST_TIME_ENTRIES) {
        refreshTodayTimeDisplay();
        refreshWeeklyTimeDisplay();
        refreshMonthlyTimeDisplay();
    }
}

void BetterTogglMainWindow::adminGlobalStorageUpdated(const QString &key)
{
    if (key == GlobalStorage::CURRENT_BILLING_MONTH) {
        emit showBusyIndicator();
        refreshBillingTimeEntries(true);
    } else if (key == GlobalStorage::PROJECTS) {
        refreshSummarizers();
    } else if (key == GlobalStorage::BILLING_SUMMARIZER) {
        refreshSummarizerDisplay();
    }
}

void BetterTogglMainWindow::configUpdate(const QString &key)
{
    if (key == SimpleConfig::WORKSPACE) {
        workspaceUpdated();
    } else if (key == SimpleConfig::DISPLAY_BILLABLE_ONLY || key == SimpleConfig::DISPLAY_ROUNDED) {
        refreshTodayTimeDisplay();
        refreshWeeklyTimeDisplay();
        refreshMonthlyTimeDisplay();
    }
}

void BetterTogglMainWindow::workspaceUpdated()
{
    auto me = _globalStorage->me();
    auto workspaceId = qvariant_cast<int>(
        _simpleConfig->get(SimpleConfig::WORKSPACE,
                           me.get<int>(toggl::Me::DEFAULT_WORKSPACE_ID).value_or(-1)));
    auto workspace = _globalStorage->workspace(workspaceId);
    if (workspace.has_value()) {
        ui->statusBar->updateWorkspace(
            workspace.value().get<QString>(toggl::Workspace::NAME).value_or(""),
            workspace.value().get<QString>(toggl::Workspace::LOGO_URL).value_or(""));
    }
}

std::tuple<toggl::Rounding, toggl::RoundingMinute, int>
BetterTogglMainWindow::getRoundingConfiguration()
{
    auto me = _globalStorage->me();
    auto workspaceId = qvariant_cast<int>(
        _simpleConfig->get(SimpleConfig::WORKSPACE,
                           me.get<int>(toggl::Me::DEFAULT_WORKSPACE_ID).value_or(-1)));
    auto workspace = _globalStorage->workspace(workspaceId);
    int roundingInt = static_cast<int>(toggl::Rounding::NO_ROUNDING);
    int roundingMinutesInt = static_cast<int>(toggl::RoundingMinute::MINUTES_0);
    if (workspace.has_value()) {
        roundingInt = workspace.value().get<int>(toggl::Workspace::ROUNDING).value_or(roundingInt);
        roundingMinutesInt = workspace.value()
                                 .get<int>(toggl::Workspace::ROUNDING_MINUTES)
                                 .value_or(roundingMinutesInt);
    }
    if (!qvariant_cast<bool>(_simpleConfig->get(SimpleConfig::USE_WORKSPACE_ROUNDING, true))) {
        roundingInt = qvariant_cast<int>(
            _simpleConfig->get(SimpleConfig::CUSTOM_ROUNDING, roundingInt));
        roundingMinutesInt = qvariant_cast<int>(
            _simpleConfig->get(SimpleConfig::CUSTOM_ROUNDING_MINUTES, roundingMinutesInt));
    }
    auto rounding = static_cast<toggl::Rounding>(roundingInt);
    auto roundingMinute = static_cast<toggl::RoundingMinute>(roundingMinutesInt);
    return {rounding, roundingMinute, workspaceId};
}

void BetterTogglMainWindow::refreshMonthlyTimeDisplay()
{
    auto [rounding, roundingMinute, workspaceId] = getRoundingConfiguration();

    auto activeMonthlyTimeEntries
        = _globalStorage->latestTimeEntries().forWorkspace(workspaceId).active().thisMonth();

    auto displayBillable = qvariant_cast<bool>(
        _simpleConfig->get(SimpleConfig::DISPLAY_BILLABLE_ONLY));
    auto displayRounded = qvariant_cast<bool>(_simpleConfig->get(SimpleConfig::DISPLAY_ROUNDED));

    if (displayBillable) {
        activeMonthlyTimeEntries = activeMonthlyTimeEntries.billable();
    }
    if (displayRounded) {
        monthlySecondsChanged(activeMonthlyTimeEntries.sum(rounding, roundingMinute));
    } else {
        monthlySecondsChanged(activeMonthlyTimeEntries.sum());
    }
}

void BetterTogglMainWindow::refreshWeeklyTimeDisplay()
{
    auto [rounding, roundingMinute, workspaceId] = getRoundingConfiguration();

    auto activeWeeklyTimeEntries
        = _globalStorage->latestTimeEntries().forWorkspace(workspaceId).active().thisWeek();

    auto displayBillable = qvariant_cast<bool>(
        _simpleConfig->get(SimpleConfig::DISPLAY_BILLABLE_ONLY));
    auto displayRounded = qvariant_cast<bool>(_simpleConfig->get(SimpleConfig::DISPLAY_ROUNDED));

    if (displayBillable) {
        activeWeeklyTimeEntries = activeWeeklyTimeEntries.billable();
    }
    if (displayRounded) {
        weeklySecondsChanged(activeWeeklyTimeEntries.sum(rounding, roundingMinute));
    } else {
        weeklySecondsChanged(activeWeeklyTimeEntries.sum());
    }
}

void BetterTogglMainWindow::refreshTodayTimeDisplay()
{
    auto [rounding, roundingMinute, workspaceId] = getRoundingConfiguration();

    auto activeTodayTimeEntries
        = _globalStorage->latestTimeEntries().forWorkspace(workspaceId).active().today();

    auto displayBillable = qvariant_cast<bool>(
        _simpleConfig->get(SimpleConfig::DISPLAY_BILLABLE_ONLY));
    auto displayRounded = qvariant_cast<bool>(_simpleConfig->get(SimpleConfig::DISPLAY_ROUNDED));

    if (displayBillable) {
        activeTodayTimeEntries = activeTodayTimeEntries.billable();
    }
    if (displayRounded) {
        todaySecondsChanged(activeTodayTimeEntries.sum(rounding, roundingMinute));
    } else {
        todaySecondsChanged(activeTodayTimeEntries.sum());
    }
}

void BetterTogglMainWindow::refreshSummarizers()
{
    auto projects = _adminGlobalStorage->projects();
    QMap<int, BillingSummarizer> loadedSummarizer;
    for (const auto &project : projects) {
        auto id = project.get<int>(toggl::Project::ID);
        if (id.has_value()) {
            QFile f(QString("%1/summarizer_%2.json")
                        .arg(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation))
                        .arg(id.value()));
            BillingSummarizer summarizer;
            if (f.open(QIODevice::ReadOnly)) {
                QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
                summarizer.fromJson(doc.object());
            }
            loadedSummarizer.insert(id.value(), summarizer);
        }
    }
    _adminGlobalStorage->set(loadedSummarizer);
}

void BetterTogglMainWindow::storeSummarizers()
{
    QMap<int, BillingSummarizer> summarizers;
    for (int i = 0; i < ui->summarizerProjectListWidget->count(); ++i) {
        auto item = ui->summarizerProjectListWidget->item(i);
        auto project = qvariant_cast<toggl::Project>(item->data(Qt::UserRole));
        auto summarizer = qvariant_cast<BillingSummarizer>(item->data(Qt::UserRole + 1));
        auto projectId = project.get<int>(toggl::Project::ID).value_or(-1);
        QFile f(QString("%1/summarizer_%2.json")
                    .arg(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation))
                    .arg(projectId));
        if (f.open(QIODevice::WriteOnly)) {
            QJsonDocument doc(summarizer.toJson());
            f.write(doc.toJson());
        }
        summarizers.insert(projectId, summarizer);
    }
    _adminGlobalStorage->set(summarizers);
}

void BetterTogglMainWindow::refreshSummarizerDisplay()
{
    auto projects = _adminGlobalStorage->projects();
    auto summarizer = _adminGlobalStorage->billingSummarizer();
    ui->summarizerProjectListWidget->clear();
    for (const auto &project : projects) {
        auto item = new QListWidgetItem();
        auto projectId = project.get<int>(toggl::Project::ID).value_or(0);
        auto projectSummarizer = summarizer.value(projectId);
        item->setData(Qt::UserRole, QVariant::fromValue(project));
        item->setData(Qt::UserRole + 1, QVariant::fromValue(projectSummarizer));
        item->setText(project.get<QString>(toggl::Project::NAME).value_or(""));
        ui->summarizerProjectListWidget->addItem(item);
    }
}

void BetterTogglMainWindow::summarizerEntryDoubleClicked(QListWidgetItem *item)
{
    auto index = ui->summarizerProjectListWidget->row(item);
    auto project = qvariant_cast<toggl::Project>(item->data(Qt::UserRole));
    auto projectId = project.get<int>(toggl::Project::ID).value_or(-1);
    QList<toggl::Task> projectTasks;
    auto tasks = _adminGlobalStorage->tasks();
    std::copy_if(std::begin(tasks),
                 std::end(tasks),
                 std::back_insert_iterator(projectTasks),
                 [=](const toggl::Task &task) {
                     return task.get<int>(toggl::Task::PROJECT_ID).value_or(-1) == projectId;
                 });
    auto summarizer = qvariant_cast<BillingSummarizer>(item->data(Qt::UserRole + 1));
    auto billingSummarizerDialog = new BillingSummarizerDialog(summarizer,
                                                               projectTasks,
                                                               _adminGlobalStorage->users(),
                                                               this);
    connect(billingSummarizerDialog,
            &BillingSummarizerDialog::saveSummarizer,
            this,
            [=](const BillingSummarizer &summarizer) {
                ui->summarizerProjectListWidget->item(index)->setData(Qt::UserRole + 1,
                                                                      QVariant::fromValue(
                                                                          summarizer));
                storeSummarizers();
            });
    connect(billingSummarizerDialog, &BillingSummarizerDialog::finished, this, [=](int) {
        billingSummarizerDialog->deleteLater();
    });
    billingSummarizerDialog->setModal(true);
    billingSummarizerDialog->open();
}

void BetterTogglMainWindow::gotoTrackingPage()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void BetterTogglMainWindow::gotoAdminPage()
{
    ui->stackedWidget->setCurrentIndex(3);
}

void BetterTogglMainWindow::gotoAnalyticsPage()
{
    ui->stackedWidget->setCurrentIndex(2);
}

void BetterTogglMainWindow::gotoBillingPage()
{
    auto conn = QSharedPointer<QMetaObject::Connection>::create();
    *conn = connect(_adminApiInterface.data(),
                    &toggl::ApiInterface::checkAuthenticationStateAnswer,
                    this,
                    [=](bool isAuthenticated) {
                        disconnect(*conn);
                        if (isAuthenticated) {
                            adminAuthenticationStateAnswer(true);
                        } else {
                            auto loginDialog = new ApiLoginDialog(_adminApiInterface,
                                                                  _simpleConfig,
                                                                  this);
                            connect(loginDialog, &QDialog::finished, this, [=](int result) {
                                if (result == QDialog::Accepted) {
                                    adminAuthenticationStateAnswer(true);
                                }
                                loginDialog->deleteLater();
                            });
                            loginDialog->setModal(true);
                            loginDialog->open();
                        }
                    });
    _adminApiInterface->checkAuthenticationState();
}

void BetterTogglMainWindow::stackedWidgetIndexChanged(int index)
{
    ui->statusBar->setVisible(index > 0);
    ui->menuView->setVisible(index > 0);
    ui->menuAccount->setVisible(index > 0);
    ui->menuDisplay->setVisible(index > 0);
    ui->actionTime_Tracking->setChecked(index == 1);
    ui->actionAnalytics->setChecked(index == 2);
    ui->actionBilling->setChecked(index == 3);
    const auto storedGeometry = _simpleConfig->get(QString("UI/Display%1").arg(index));
    if (!storedGeometry.isNull() && storedGeometry.isValid()) {
        setGeometry(qvariant_cast<QRect>(storedGeometry));
    }
}

void BetterTogglMainWindow::authenticationStateAnswer(bool isAuthenticated)
{
    if (isAuthenticated) {
        ui->stackedWidget->setCurrentIndex(1);
        refreshData();
    } else {
        if (ui->stackedWidget->currentIndex() == 0) {
            launchLoginDialog();
        }
    }
}

void BetterTogglMainWindow::adminAuthenticationStateAnswer(bool isAuthenticated)
{
    if (isAuthenticated) {
        ui->stackedWidget->setCurrentIndex(3);
        refreshBillingData();
    }
}

void BetterTogglMainWindow::launchLoginDialog()
{
    auto loginDialog = new LoginDialog(_apiInterface, _persistentStorage, this);
    connect(loginDialog, &QDialog::finished, this, [=](int result) {
        if (result == QDialog::Accepted) {
            authenticationStateAnswer(true);
        }
        loginDialog->deleteLater();
    });
    loginDialog->setModal(true);
    loginDialog->open();
}

void BetterTogglMainWindow::logout()
{
    auto logoutDialog = new LogoutDialog(this);

    connect(logoutDialog, &QDialog::finished, this, [=](int result) {
        if (result == QDialog::Accepted) {
            _apiInterface->logout();
        }
        logoutDialog->deleteLater();
    });

    logoutDialog->setModal(true);
    logoutDialog->open();
}

void BetterTogglMainWindow::logoutAnswer()
{
    _globalStorage->clear();
    _adminGlobalStorage->clear();
    _simpleConfig->clearAuthenticatedData();
    ui->stackedWidget->setCurrentIndex(0);
}

void BetterTogglMainWindow::updateTimeDisplay(QLabel *displayLabel, qint64 seconds)
{
    QStringList timeComponents;
    qint64 hours = qFloor(seconds / 3600.0);
    seconds = seconds % 3600;
    qint64 minutes = qFloor(seconds / 60);
    qint64 displaySeconds = seconds % 60;
    if (hours > 0) {
        timeComponents.append(QString::number(hours));
    }
    timeComponents.append(QString::number(minutes).rightJustified(2, '0'));
    timeComponents.append(QString::number(displaySeconds).rightJustified(2, '0'));
    displayLabel->setText(timeComponents.join(":").rightJustified(9));
}

void BetterTogglMainWindow::todaySecondsChanged(qint64 totalSeconds)
{
    updateTimeDisplay(ui->todayTimeLabel, totalSeconds);
}

void BetterTogglMainWindow::weeklySecondsChanged(qint64 totalSeconds)
{
    updateTimeDisplay(ui->thisWeekTimeLabel, totalSeconds);
}

void BetterTogglMainWindow::monthlySecondsChanged(qint64 totalSeconds)
{
    updateTimeDisplay(ui->thisMonthTimeLabel, totalSeconds);
}

void BetterTogglMainWindow::detailsAnswer(const toggl::TimeEntryList &timeEntries)
{
    _globalStorage->set(GlobalStorage::DETAILS, QVariant::fromValue(timeEntries));
}

void BetterTogglMainWindow::adminDetailsAnswer(const toggl::TimeEntryList &timeEntries)
{
    _adminGlobalStorage->set(GlobalStorage::DETAILS, QVariant::fromValue(timeEntries));
}

void BetterTogglMainWindow::switchWorkspaceDialogClicked()
{
    auto dialog = new SwitchWorkspaceDialog(_globalStorage, _simpleConfig, this);
    connect(dialog, &SwitchWorkspaceDialog::finished, this, [=](int result) {
        dialog->deleteLater();
    });
    dialog->setModal(true);
    dialog->open();
}

void BetterTogglMainWindow::showOnlyRounded(bool checked)
{
    _simpleConfig->set(SimpleConfig::DISPLAY_ROUNDED, checked);
}

void BetterTogglMainWindow::showOnlyBillable(bool checked)
{
    _simpleConfig->set(SimpleConfig::DISPLAY_BILLABLE_ONLY, checked);
}

void BetterTogglMainWindow::refreshData()
{
    emit updateBusyIndicatorMessage(tr("Refreshing data ..."));
    emit showBusyIndicator();
    refreshMe();
}

void BetterTogglMainWindow::refreshMe()
{
    auto conn = QSharedPointer<QMetaObject::Connection>::create();
    *conn = connect(_apiInterface.data(),
                    &toggl::ApiInterface::meAnswer,
                    this,
                    [=](const toggl::Me &) {
                        disconnect(*conn);
                        refreshWorkspaces();
                    });
    _apiInterface->me(false);
}

void BetterTogglMainWindow::refreshWorkspaces()
{
    auto conn = QSharedPointer<QMetaObject::Connection>::create();
    *conn = connect(_apiInterface.data(),
                    &toggl::ApiInterface::workspacesAnswer,
                    this,
                    [=](const QList<toggl::Workspace> &) {
                        disconnect(*conn);
                        refreshClients();
                    });
    _apiInterface->workspaces();
}

void BetterTogglMainWindow::refreshClients()
{
    auto me = _globalStorage->me();
    auto workspaceId = qvariant_cast<int>(
        _simpleConfig->get(SimpleConfig::WORKSPACE,
                           me.get<int>(toggl::Me::DEFAULT_WORKSPACE_ID).value_or(-1)));
    auto conn = QSharedPointer<QMetaObject::Connection>::create();
    *conn = connect(_apiInterface.data(),
                    &toggl::ApiInterface::reportClientsAnswer,
                    this,
                    [=](const QList<toggl::Client> &) {
                        disconnect(*conn);
                        refreshProjects();
                    });
    _apiInterface->reportClients(workspaceId);
}

void BetterTogglMainWindow::refreshProjects()
{
    auto me = _globalStorage->me();
    auto workspaceId = qvariant_cast<int>(
        _simpleConfig->get(SimpleConfig::WORKSPACE,
                           me.get<int>(toggl::Me::DEFAULT_WORKSPACE_ID).value_or(-1)));
    auto conn = QSharedPointer<QMetaObject::Connection>::create();
    *conn = connect(_apiInterface.data(),
                    &toggl::ApiInterface::reportProjectsAnswer,
                    this,
                    [=](const QList<toggl::Project> &) {
                        disconnect(*conn);
                        refreshTasks();
                    });
    _apiInterface->reportProjects(workspaceId);
}

void BetterTogglMainWindow::refreshTasks()
{
    auto me = _globalStorage->me();
    auto workspaceId = qvariant_cast<int>(
        _simpleConfig->get(SimpleConfig::WORKSPACE,
                           me.get<int>(toggl::Me::DEFAULT_WORKSPACE_ID).value_or(-1)));
    auto conn = QSharedPointer<QMetaObject::Connection>::create();
    *conn = connect(_apiInterface.data(),
                    &toggl::ApiInterface::reportTasksAnswer,
                    this,
                    [=](const QList<toggl::Task> &) {
                        disconnect(*conn);
                        refreshLatestTimeEntries();
                    });
    _apiInterface->reportTasks(workspaceId);
}

void BetterTogglMainWindow::refreshLatestTimeEntries()
{
    auto conn = QSharedPointer<QMetaObject::Connection>::create();
    *conn = connect(_apiInterface.data(),
                    &toggl::ApiInterface::timeEntriesLatestAnswer,
                    this,
                    [=](const toggl::TimeEntryList &) {
                        disconnect(*conn);
                        emit hideBusyIndicator();
                    });
    _apiInterface->timeEntriesLatest();
}

void BetterTogglMainWindow::refreshBillingData()
{
    emit updateBusyIndicatorMessage(tr("Refreshing data ..."));
    emit showBusyIndicator();
    refreshBillingUsers();
}

void BetterTogglMainWindow::refreshBillingUsers()
{
    auto me = _globalStorage->me();
    auto workspaceId = qvariant_cast<int>(
        _simpleConfig->get(SimpleConfig::WORKSPACE,
                           me.get<int>(toggl::Me::DEFAULT_WORKSPACE_ID).value_or(-1)));
    auto conn = QSharedPointer<QMetaObject::Connection>::create();
    *conn = connect(_adminApiInterface.data(),
                    &toggl::ApiInterface::reportUsersAnswer,
                    this,
                    [=](const QList<toggl::ReportUser> &) {
                        disconnect(*conn);
                        refreshBillingClients();
                    });
    _adminApiInterface->reportUsers(workspaceId);
}

void BetterTogglMainWindow::refreshBillingClients()
{
    auto me = _globalStorage->me();
    auto workspaceId = qvariant_cast<int>(
        _simpleConfig->get(SimpleConfig::WORKSPACE,
                           me.get<int>(toggl::Me::DEFAULT_WORKSPACE_ID).value_or(-1)));
    auto conn = QSharedPointer<QMetaObject::Connection>::create();
    *conn = connect(_adminApiInterface.data(),
                    &toggl::ApiInterface::reportClientsAnswer,
                    this,
                    [=](const QList<toggl::Client> &) {
                        disconnect(*conn);
                        refreshBillingProjects();
                    });
    _adminApiInterface->reportClients(workspaceId);
}

void BetterTogglMainWindow::refreshBillingProjects()
{
    auto me = _globalStorage->me();
    auto workspaceId = qvariant_cast<int>(
        _simpleConfig->get(SimpleConfig::WORKSPACE,
                           me.get<int>(toggl::Me::DEFAULT_WORKSPACE_ID).value_or(-1)));
    auto conn = QSharedPointer<QMetaObject::Connection>::create();
    *conn = connect(_adminApiInterface.data(),
                    &toggl::ApiInterface::reportProjectsAnswer,
                    this,
                    [=](const QList<toggl::Project> &) {
                        disconnect(*conn);
                        refreshBillingTasks();
                    });
    _adminApiInterface->reportProjects(workspaceId);
}

void BetterTogglMainWindow::refreshBillingTasks()
{
    auto me = _globalStorage->me();
    auto workspaceId = qvariant_cast<int>(
        _simpleConfig->get(SimpleConfig::WORKSPACE,
                           me.get<int>(toggl::Me::DEFAULT_WORKSPACE_ID).value_or(-1)));
    auto conn = QSharedPointer<QMetaObject::Connection>::create();
    *conn = connect(_adminApiInterface.data(),
                    &toggl::ApiInterface::reportTasksAnswer,
                    this,
                    [=](const QList<toggl::Task> &) {
                        disconnect(*conn);
                        refreshBillingTimeEntries(true);
                    });
    _adminApiInterface->reportTasks(workspaceId);
}

void BetterTogglMainWindow::refreshBillingTimeEntries(bool hideBusyIndicatorAfterExecution)
{
    auto me = _globalStorage->me();
    auto workspaceId = qvariant_cast<int>(
        _simpleConfig->get(SimpleConfig::WORKSPACE,
                           me.get<int>(toggl::Me::DEFAULT_WORKSPACE_ID).value_or(-1)));
    auto conn = QSharedPointer<QMetaObject::Connection>::create();
    *conn = connect(_adminApiInterface.data(),
                    &toggl::ApiInterface::detailsAnswer,
                    this,
                    [=](const toggl::TimeEntryList &) {
                        disconnect(*conn);
                        if (hideBusyIndicatorAfterExecution) {
                            emit hideBusyIndicator();
                        }
                    });

    const QDate currentMonthDate = qvariant_cast<QDate>(
        _adminGlobalStorage->get(GlobalStorage::CURRENT_BILLING_MONTH, QDate::currentDate()));

    ui->billingMonthSelection->setText(currentMonthDate.toString("MM/yyyy"));

    const QDate startDate(currentMonthDate.year(), currentMonthDate.month(), 1);
    const QDate endDate(currentMonthDate.year(),
                        currentMonthDate.month(),
                        currentMonthDate.daysInMonth());
    _adminApiInterface->details(workspaceId,
                                startDate,
                                endDate,
                                _adminGlobalStorage->users(),
                                _adminGlobalStorage->clients(),
                                _adminGlobalStorage->projects(),
                                _adminGlobalStorage->tasks());
}

void BetterTogglMainWindow::hideEmptyProjectsToggled(int state)
{
    _simpleConfig->set(SimpleConfig::BILLING_HIDE_EMPTY_PROJECTS,
                       ui->hideEmptyProjectsCheckBox->isChecked());
}

void BetterTogglMainWindow::billingPreviousMonth()
{
    QDate currentMonthDate = qvariant_cast<QDate>(
        _adminGlobalStorage->get(GlobalStorage::CURRENT_BILLING_MONTH, QDate::currentDate()));
    currentMonthDate.setDate(currentMonthDate.year(), currentMonthDate.month(), 1);
    currentMonthDate = currentMonthDate.addMonths(-1);
    _adminGlobalStorage->set(GlobalStorage::CURRENT_BILLING_MONTH, currentMonthDate);
}

void BetterTogglMainWindow::billingNextMonth()
{
    QDate currentMonthDate = qvariant_cast<QDate>(
        _adminGlobalStorage->get(GlobalStorage::CURRENT_BILLING_MONTH, QDate::currentDate()));
    currentMonthDate.setDate(currentMonthDate.year(), currentMonthDate.month(), 1);
    currentMonthDate = currentMonthDate.addMonths(1);
    _adminGlobalStorage->set(GlobalStorage::CURRENT_BILLING_MONTH, currentMonthDate);
}

void BetterTogglMainWindow::summaryTreeViewEntryDoubleClicked(const QModelIndex &index)
{
    if (!_summarizedTimeEntryModel->data(index, SummarizedTimeEntryModel::IsEntryItem).toBool()) {
        return;
    }
    auto summarizedTimeEntry = qvariant_cast<BillingSummarizer::SummarizedTimeEntry>(
        _summarizedTimeEntryModel->data(index, SummarizedTimeEntryModel::EntryItem));
    auto d = new TimeEntryViewerDialog(summarizedTimeEntry.summarizedEntries,
                                       _adminGlobalStorage->clients(),
                                       _adminGlobalStorage->projects(),
                                       _adminGlobalStorage->tasks(),
                                       _adminGlobalStorage->users(),
                                       this);

    auto conn = QSharedPointer<QMetaObject::Connection>::create();
    *conn = connect(d, &QDialog::finished, this, [=](int) {
        disconnect(*conn);
        d->deleteLater();
    });
    d->setModal(true);
    d->open();
}

void BetterTogglMainWindow::settingsClicked()
{
    auto d = new SettingsDialog(_simpleConfig, this);
    auto conn = QSharedPointer<QMetaObject::Connection>::create();
    *conn = connect(d, &SettingsDialog::finished, this, [=](int) {
        disconnect(*conn);
        d->deleteLater();
    });
    d->setModal(true);
    d->open();
}

void BetterTogglMainWindow::exportBillingData()
{
    auto d = new ExportDialog(_summarizedTimeEntryModel->getExportData(),
                              _simpleConfig,
                              _adminGlobalStorage,
                              this);

    auto conn = QSharedPointer<QMetaObject::Connection>::create();
    *conn = connect(d, &ExportDialog::finished, this, [=](int) {
        disconnect(*conn);
        d->deleteLater();
    });

    d->setModal(true);
    d->open();
}
