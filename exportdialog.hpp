#ifndef EXPORTDIALOG_HPP
#define EXPORTDIALOG_HPP

#include <QDialog>
#include <QSharedPointer>

#include "billingsummarizer.hpp"
#include "busyindicatordialog.hpp"

class SimpleConfig;

namespace Ui {
    class ExportDialog;
}

class ExportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExportDialog(const QMap<QPair<QString, QString>,
                                     QList<BillingSummarizer::SummarizedTimeEntry>> &exportData,
                          QSharedPointer<SimpleConfig> simpleConfig,
                          QSharedPointer<GlobalStorage> globalStorage,
                          QWidget *parent = nullptr);
    ~ExportDialog();

public slots:
    void startExport();

    void processCurrentIndex();

    void processingFinished();

private:
    Ui::ExportDialog *ui;
    QMap<QPair<QString, QString>, QList<BillingSummarizer::SummarizedTimeEntry>> _exportData;
    QSharedPointer<SimpleConfig> _simpleConfig;
    QSharedPointer<GlobalStorage> _globalStorage;
    BusyIndicatorDialog *_busyIndicatorDialog;

    QMap<int, QString> _processingErrors;

    int _processingIndex = 0;
};

#endif // EXPORTDIALOG_HPP
