#ifndef BILLINGEXPORTER_HPP
#define BILLINGEXPORTER_HPP

#include <QObject>

#include "billingsummarizer.hpp"

class BillingExporter : public QObject
{
    Q_OBJECT
public:
    explicit BillingExporter(QObject *parent = nullptr);

    virtual void exportBillingData(
        const QList<BillingSummarizer::SummarizedTimeEntry> &summarizedData)
        = 0;

signals:
    void finished(bool success, const QString &error = QString());
};

#endif // BILLINGEXPORTER_HPP
