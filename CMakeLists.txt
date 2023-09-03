cmake_minimum_required(VERSION 3.5)

if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
  find_package(Git)
  if(GIT_FOUND)
    execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --short=8 HEAD
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      OUTPUT_VARIABLE GIT_HASH
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )
  endif()
endif()

set(BASE_VERSION 1.0.0)
set(BASE_VERSION_META -alpha)
set(APP_NAME "Better Toggl")
set(APP_AUTHOR "Artem Dontsov")
set(APP_AUTHOR_MAIL "artem@dontsoft.com")
set(APP_AUTHOR_WEBPAGE "https://dontsoft.com")
set(APP_AUTHOR_GITHUB "https://github.com/dontsoft")
set(APP_COMMENT "")
set(APP_DESCRIPTION "")
set(APP_YEAR "2023")


if(DEFINED GIT_HASH)
    message("-- Git hash = ${GIT_HASH}")
    list(APPEND DEFINITIONS -DGITHASH="${GIT_HASH}")
    set(APP_VERSION ${BASE_VERSION}${BASE_VERSION_META}+sha.${GIT_HASH})
else()
    set(GIT_HASH "Unknown")
    message("-- Git hash = N/A")
    set(APP_VERSION ${BASE_VERSION}${BASE_VERSION_META}+sha.unknown)
endif()

if(BASE_VERSION MATCHES "^([0-9]+)\\.([0-9]+)\\.([0-9]+)")
  set(VERSION_MAJOR "${CMAKE_MATCH_1}")
  set(VERSION_MINOR "${CMAKE_MATCH_2}")
  set(VERSION_PATCH "${CMAKE_MATCH_3}")
  set(WINDOWS_VERSION ${VERSION_MAJOR},${VERSION_MINOR},${VERSION_PATCH})
else()
  message(FATAL_ERROR "Failed to parse BASE_VERSION='${BASE_VERSION}'")
endif()


project(BetterToggl VERSION ${BASE_VERSION} LANGUAGES CXX)

configure_file(build.h.in build.hpp @ONLY)
configure_file(version.rc.in version.rc @ONLY)
configure_file(${PROJECT_NAME}.rc.in ${PROJECT_NAME}.rc @ONLY)

message("-- Version = ${PROJECT_VERSION}")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network Sql)

set(PROJECT_SOURCES
        main.cpp
        qaesencryption.cpp qaesencryption.h
        bettertogglmainwindow.cpp
        bettertogglmainwindow.hpp
        bettertogglmainwindow.ui
        busyindicator.hpp busyindicator.cpp
        busyindicatordialog.hpp busyindicatordialog.cpp busyindicatordialog.ui
        toggl/apiinterface.hpp toggl/apiinterface.cpp
        toggl/apiendpoints.hpp
        globalstorage.hpp globalstorage.cpp
        persistentstorage.hpp persistentstorage.cpp
        logindialog.hpp logindialog.cpp logindialog.ui
        simpleconfig.hpp simpleconfig.cpp
        global.hpp
        logoutdialog.hpp logoutdialog.cpp logoutdialog.ui
        statusbar.hpp statusbar.cpp statusbar.ui
        imageviewer.hpp imageviewer.cpp
        switchworkspacedialog.hpp switchworkspacedialog.cpp switchworkspacedialog.ui
        fonts.qrc icons.qrc
        toggl/timeentrylist.hpp toggl/timeentrylist.cpp
        toggl/apitypes.hpp toggl/apitypes.cpp
        toggl/apiconstants.hpp
        theme.hpp theme.cpp lighttheme.hpp lighttheme.cpp
        darktheme.hpp darktheme.cpp
        apilogindialog.hpp apilogindialog.cpp apilogindialog.ui
        billingsummarizer.hpp billingsummarizer.cpp
        billingsummarizerwidget.hpp billingsummarizerwidget.cpp billingsummarizerwidget.ui
        splitbyrulewidget.hpp splitbyrulewidget.cpp splitbyrulewidget.ui
        sortablelistlayout.hpp sortablelistlayout.cpp
        billingsummarizerdialog.hpp billingsummarizerdialog.cpp billingsummarizerdialog.ui
        summarizedtimeentrymodel.hpp summarizedtimeentrymodel.cpp
        timeentryviewer.hpp timeentryviewer.cpp timeentryviewer.ui
        timeentryviewerdialog.hpp timeentryviewerdialog.cpp timeentryviewerdialog.ui
        billingexporter.hpp billingexporter.cpp
        exporter/excelbillingexporter.hpp exporter/excelbillingexporter.cpp
        settingsdialog.hpp settingsdialog.cpp settingsdialog.ui
        exportdialog.hpp exportdialog.cpp exportdialog.ui
)

if (WIN32)

    set(app_icon_resource_windows "${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.rc")
    qt_add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${app_icon_resource_windows})

    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
    )

elseif (APPLE)
    set(MACOSX_BUNDLE_ICON_FILE icon.icns)
    set(app_icon_macos "${CMAKE_CURRENT_SOURCE_DIR}/icons/icon.icns")
    set_source_files_properties(${app_icon_macos} PROPERTIES
               MACOSX_PACKAGE_LOCATION "Resources")
    qt_add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${PROJECT_SOURCES} ${app_icon_macos}
        )

    set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER com.dontsoft.bettertoggl
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE_BUNDLE_NAME ${APP_NAME}
        MACOSX_BUNDLE TRUE
    )
else()
    qt_add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
endif()

add_subdirectory(QXlsx/QXlsx)
target_link_libraries(${PROJECT_NAME} PRIVATE QXlsx::QXlsx)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network Qt${QT_VERSION_MAJOR}::Sql)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${APP_NAME})

install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
