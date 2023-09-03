#include "exportdialog.hpp"
#include "ui_exportdialog.h"

#include <QDir>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QTableWidgetItem>

#include "exporter/excelbillingexporter.hpp"
#include "globalstorage.hpp"
#include "simpleconfig.hpp"

ExportDialog::ExportDialog(
    const QMap<QPair<QString, QString>, QList<BillingSummarizer::SummarizedTimeEntry>> &exportData,
    QSharedPointer<SimpleConfig> simpleConfig,
    QSharedPointer<GlobalStorage> globalStorage,
    QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ExportDialog)
    , _exportData(exportData)
    , _simpleConfig(simpleConfig)
    , _globalStorage(globalStorage)
    , _busyIndicatorDialog(new BusyIndicatorDialog(this))
{
    ui->setupUi(this);
    _busyIndicatorDialog->setModal(true);
    connect(ui->cancelButton, &QPushButton::clicked, this, &ExportDialog::reject);
    connect(ui->startExportButton, &QPushButton::clicked, this, &ExportDialog::startExport);

    ui->exportOverviewWidget->setColumnCount(4);
    ui->exportOverviewWidget->setHorizontalHeaderLabels(
        {tr("Project"), tr("Group"), tr("Export Path"), tr("Error")});
    for (auto it = _exportData.begin(); it != _exportData.end(); ++it) {
        ui->exportOverviewWidget->insertRow(ui->exportOverviewWidget->rowCount());
        auto projectItem = new QTableWidgetItem(it.key().first);
        projectItem->setFlags((projectItem->flags() | Qt::ItemIsUserCheckable)
                              & ~Qt::ItemIsEditable);
        projectItem->setCheckState(Qt::Checked);
        projectItem->setData(Qt::UserRole, QVariant::fromValue(it.value()));
        auto groupItem = new QTableWidgetItem(it.key().second);
        groupItem->setFlags(groupItem->flags() & ~Qt::ItemIsEditable);
        static QRegularExpression invalid_chars("[/\\$]");
        auto filename = QString("%1_%2").arg(it.key().first, it.key().second);
        filename.replace(invalid_chars, "_");

        auto path = QString("%1/%2/%3.xlsx")
                        .arg(_simpleConfig
                                 ->get(SimpleConfig::EXPORT_DEFAULT_LOCATION,
                                       QStandardPaths::writableLocation(
                                           QStandardPaths::DocumentsLocation))
                                 .toString(),
                             _globalStorage
                                 ->get(GlobalStorage::CURRENT_BILLING_MONTH,
                                       QDateTime::currentDateTime())
                                 .toDateTime()
                                 .toString("yyyy-MM"),
                             filename);
        auto pathItem = new QTableWidgetItem(path);
        pathItem->setFlags(pathItem->flags() | Qt::ItemIsEditable);
        auto errorItem = new QTableWidgetItem("");
        errorItem->setFlags(groupItem->flags() & ~Qt::ItemIsEditable);
        ui->exportOverviewWidget->setItem(ui->exportOverviewWidget->rowCount() - 1, 0, projectItem);
        ui->exportOverviewWidget->setItem(ui->exportOverviewWidget->rowCount() - 1, 1, groupItem);
        ui->exportOverviewWidget->setItem(ui->exportOverviewWidget->rowCount() - 1, 2, pathItem);
        ui->exportOverviewWidget->setItem(ui->exportOverviewWidget->rowCount() - 1, 3, errorItem);
    }
    ui->exportOverviewWidget->resizeColumnToContents(0);
    ui->exportOverviewWidget->resizeColumnToContents(1);
    ui->exportOverviewWidget->resizeColumnToContents(2);
}

ExportDialog::~ExportDialog()
{
    delete ui;
}

void ExportDialog::startExport()
{
    _busyIndicatorDialog->show();
    _processingErrors.clear();
    _processingIndex = 0;

    processCurrentIndex();
}

void ExportDialog::processCurrentIndex()
{
    if (_processingIndex >= ui->exportOverviewWidget->rowCount()) {
        _busyIndicatorDialog->hide();
        processingFinished();
        return;
    }
    auto pathItem = ui->exportOverviewWidget->item(_processingIndex, 2);
    auto dataItem = ui->exportOverviewWidget->item(_processingIndex, 0);
    if (pathItem && dataItem && dataItem->checkState() != Qt::Unchecked) {
        auto path = pathItem->data(Qt::DisplayRole).toString();
        if (!QFileInfo(path).absoluteDir().exists()) {
            QDir().mkpath(QFileInfo(path).absolutePath());
        }
        auto exporter = new ExcelBillingExporter(path);
        exporter->setTitle(_simpleConfig->get(SimpleConfig::EXPORT_TITLE).toString());
        exporter->setSubject(_simpleConfig->get(SimpleConfig::EXPORT_SUBJECT).toString());
        exporter->setCreator(_simpleConfig->get(SimpleConfig::EXPORT_CREATOR).toString());
        exporter->setCompany(_simpleConfig->get(SimpleConfig::EXPORT_COMPANY).toString());
        exporter->setDescription(_simpleConfig->get(SimpleConfig::EXPORT_DESCRIPTION).toString());
        auto data = qvariant_cast<QList<BillingSummarizer::SummarizedTimeEntry>>(
            dataItem->data(Qt::UserRole));

        auto conn = QSharedPointer<QMetaObject::Connection>::create();
        *conn = connect(exporter,
                        &ExcelBillingExporter::finished,
                        this,
                        [=](bool success, const QString &error) {
                            disconnect(*conn);
                            if (!success) {
                                _processingErrors.insert(_processingIndex, error);
                            }
                            _processingIndex++;
                            processCurrentIndex();
                            exporter->deleteLater();
                        });

        exporter->exportBillingData(data);
    } else {
        _processingIndex++;
        processCurrentIndex();
    }
}

void ExportDialog::processingFinished()
{
    for (int i = 0; i < ui->exportOverviewWidget->rowCount(); ++i) {
        if (_processingErrors.contains(i)) {
            ui->exportOverviewWidget->item(i, 3)->setText(
                tr("ERROR! %1").arg(_processingErrors.value(i)));
        } else {
            ui->exportOverviewWidget->item(i, 3)->setText(
                tr("SUCCESS! Exported to provided path."));
        }
    }
}
