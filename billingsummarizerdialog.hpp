#ifndef BILLINGSUMMARIZERDIALOG_HPP
#define BILLINGSUMMARIZERDIALOG_HPP

#include <QDialog>

#include "billingsummarizer.hpp"
#include "toggl/apitypes.hpp"

namespace Ui {
    class BillingSummarizerDialog;
}

class BillingSummarizerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BillingSummarizerDialog(const BillingSummarizer &summarizer,
                                     const QList<toggl::Task> &tasks,
                                     const QList<toggl::ReportUser> &users,
                                     QWidget *parent = nullptr);
    ~BillingSummarizerDialog();

signals:
    void saveSummarizer(const BillingSummarizer &summarizer);

private:
    Ui::BillingSummarizerDialog *ui;
};

#endif // BILLINGSUMMARIZERDIALOG_HPP
