#ifndef EXCELBILLINGEXPORTER_HPP
#define EXCELBILLINGEXPORTER_HPP

#include <billingexporter.hpp>
#include <QObject>

class ExcelBillingExporter : public BillingExporter
{
    Q_OBJECT
public:
    explicit ExcelBillingExporter(const QString &fileName, QObject *parent = nullptr);

    void exportBillingData(
        const QList<BillingSummarizer::SummarizedTimeEntry> &summarizedData) override;

    void setTitle(const QString &newTitle);

    void setSubject(const QString &newSubject);

    void setCreator(const QString &newCreator);

    void setCompany(const QString &newCompany);

    void setDescription(const QString &newDescription);

private:
    const QString _fileName;
    QString _title;
    QString _subject;
    QString _creator;
    QString _company;
    QString _description;
};

#endif // EXCELBILLINGEXPORTER_HPP
