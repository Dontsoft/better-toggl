#include "billingsummarizerdialog.hpp"
#include "ui_billingsummarizerdialog.h"

BillingSummarizerDialog::BillingSummarizerDialog(const BillingSummarizer &summarizer,
                                                 const QList<toggl::Task> &tasks,
                                                 const QList<toggl::ReportUser> &users,
                                                 QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::BillingSummarizerDialog)
{
    ui->setupUi(this);

    ui->summarizerWidget->setUsers(users);
    ui->summarizerWidget->setTasks(tasks);
    ui->summarizerWidget->setSummarizer(summarizer);
    connect(ui->cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(ui->saveButton, &QPushButton::clicked, this, [=]() {
        emit saveSummarizer(ui->summarizerWidget->getSummarizer());
        accept();
    });
}

BillingSummarizerDialog::~BillingSummarizerDialog()
{
    delete ui;
}
