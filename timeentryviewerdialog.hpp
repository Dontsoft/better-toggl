#ifndef TIMEENTRYVIEWERDIALOG_HPP
#define TIMEENTRYVIEWERDIALOG_HPP

#include <QDialog>
#include "toggl/timeentrylist.hpp"

namespace Ui {
    class TimeEntryViewerDialog;
}

class TimeEntryViewerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TimeEntryViewerDialog(const toggl::TimeEntryList &list,
                                   const QList<toggl::Client> &clients,
                                   const QList<toggl::Project> &projects,
                                   const QList<toggl::Task> &tasks,
                                   const QList<toggl::ReportUser> &user,
                                   QWidget *parent = nullptr);
    ~TimeEntryViewerDialog();

private:
    Ui::TimeEntryViewerDialog *ui;
};

#endif // TIMEENTRYVIEWERDIALOG_HPP
