#include "bettertogglmainwindow.hpp"

#include <QApplication>
#include <QNetworkAccessManager>
#include <QSharedPointer>

#include "billingsummarizer.hpp"
#include "build.hpp"
#include "darktheme.hpp"
#include "global.hpp"
#include "globalstorage.hpp"
#include "lighttheme.hpp"
#include "persistentstorage.hpp"
#include "simpleconfig.hpp"
#include "toggl/apiinterface.hpp"
#include "toggl/timeentrylist.hpp"

#include <QFontDatabase>

int main(int argc, char *argv[])
{
    qRegisterMetaType<toggl::Me>("toggl::Me");
    qRegisterMetaType<toggl::Workspace>("toggl::Workspace");
    qRegisterMetaType<toggl::TimeEntry>("toggl::TimeEntry");
    qRegisterMetaType<toggl::TimeEntryList>("toggl::TimeEntryList");
    qRegisterMetaType<toggl::ReportUser>("toggl::ReportUser");
    qRegisterMetaType<toggl::Client>("toggl::Client");
    qRegisterMetaType<toggl::Project>("toggl::Project");
    qRegisterMetaType<toggl::Task>("toggl::Task");
    qRegisterMetaType<BillingSummarizer>("BillingSummarizer");

    QApplication a(argc, argv);
    LightTheme lightTheme;
    DarkTheme darkTheme;
    int returnCode = 0;

    do {
        QNetworkAccessManager manager;
        QNetworkAccessManager adminManager;
        QSharedPointer<PersistentStorage> persistentStorage
            = QSharedPointer<PersistentStorage>::create();
        QSharedPointer<GlobalStorage> globalStorage = QSharedPointer<GlobalStorage>::create();
        QSharedPointer<GlobalStorage> adminGlobalStorage = QSharedPointer<GlobalStorage>::create();
        QSharedPointer<SimpleConfig> simpleConfig = QSharedPointer<SimpleConfig>::create();
        QSharedPointer<toggl::ApiInterface> apiInterface
            = QSharedPointer<toggl::ApiInterface>::create(&manager, persistentStorage);
        QSharedPointer<toggl::ApiInterface> adminApiInterface
            = QSharedPointer<toggl::ApiInterface>::create(&adminManager,
                                                          persistentStorage,
                                                          "__Host-timer-session-ADM");
        const auto theme = simpleConfig->get(SimpleConfig::UI_THEME, SimpleConfig::Theme::LightTheme)
                               .value<SimpleConfig::Theme>();

        if (theme == SimpleConfig::Theme::LightTheme) {
            a.setPalette(lightTheme.palette());
            a.setStyleSheet(lightTheme.stylesheet());
            a.setStyle(lightTheme.style());
        } else {
            a.setPalette(darkTheme.palette());
            a.setStyleSheet(darkTheme.stylesheet());
            a.setStyle(darkTheme.style());
        }

        QApplication::setApplicationName(APP_NAME);
        QApplication::setApplicationDisplayName(APP_NAME);
        QApplication::setApplicationVersion(
            QString("%1.%2.%3").arg(VERSION_MAJOR).arg(VERSION_MINOR).arg(VERSION_PATCH));

        QFontDatabase::addApplicationFont(":/fonts/FiraMono-Regular");

        BetterTogglMainWindow w(apiInterface,
                                adminApiInterface,
                                globalStorage,
                                adminGlobalStorage,
                                persistentStorage,
                                simpleConfig);
        w.show();
        returnCode = a.exec();
    } while (returnCode == better_toggl::RESTART_RETURN_CODE);
    return returnCode;
}
