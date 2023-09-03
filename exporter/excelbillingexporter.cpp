#include "excelbillingexporter.hpp"

#include "xlsxdocument.h"

ExcelBillingExporter::ExcelBillingExporter(const QString &fileName, QObject *parent)
    : BillingExporter{parent}
    , _fileName(fileName)

{}

void ExcelBillingExporter::exportBillingData(
    const QList<BillingSummarizer::SummarizedTimeEntry> &summarizedData)
{
    try {
        QXlsx::Document doc;

        doc.setDocumentProperty("title", _title);
        doc.setDocumentProperty("subject", _subject);
        doc.setDocumentProperty("creator", _creator);
        doc.setDocumentProperty("company", _company);
        doc.setDocumentProperty("description", _description);

        auto make_border = [](QXlsx::Format format,
                              QXlsx::Format::BorderStyle style,
                              std::tuple<bool, bool, bool, bool> border) {
            auto top = std::get<0>(border);
            auto right = std::get<1>(border);
            auto bottom = std::get<2>(border);
            auto left = std::get<3>(border);
            if (top) {
                format.setTopBorderStyle(style);
            }
            if (right) {
                format.setRightBorderStyle(style);
            }
            if (bottom) {
                format.setBottomBorderStyle(style);
            }
            if (left) {
                format.setLeftBorderStyle(style);
            }

            return format;
        };

        QXlsx::Format boldCenterFormat;
        boldCenterFormat.setFontBold(true);
        boldCenterFormat.setHorizontalAlignment(QXlsx::Format::AlignHCenter);

        QXlsx::Format boldFormat;
        boldFormat.setFontBold(true);

        QXlsx::Format decimalFormat;
        decimalFormat.setNumberFormat("# ##0.000");

        QXlsx::Format decimalBoldFormat;
        decimalBoldFormat.setNumberFormat("# ##0.000");
        decimalBoldFormat.setFontBold(true);

        QXlsx::Format timeEntryFormat;
        timeEntryFormat.setNumberFormat("[hh]:mm:ss");

        QXlsx::Format timeEntryBoldFormat;
        timeEntryBoldFormat.setNumberFormat("[hh]:mm:ss");
        timeEntryBoldFormat.setFontBold(true);

        // Write header

        doc.write(1,
                  1,
                  "Date",
                  make_border(boldCenterFormat,
                              QXlsx::Format::BorderMedium,
                              {true, false, true, true}));
        doc.write(1,
                  2,
                  "Time (h)",
                  make_border(boldCenterFormat,
                              QXlsx::Format::BorderMedium,
                              {true, false, true, false}));
        doc.write(1,
                  3,
                  "Time (decimal)",
                  make_border(boldCenterFormat,
                              QXlsx::Format::BorderMedium,
                              {true, false, true, false}));
        doc.write(1,
                  4,
                  "Project/Time entry",
                  make_border(boldCenterFormat,
                              QXlsx::Format::BorderMedium,
                              {true, true, true, false}));

        // Write content
        for (int i = 0; i < summarizedData.count(); ++i) {
            if (summarizedData[i].timeFrameKeyDisplay
                == BillingSummarizer::SummarizedTimeEntry::TimeFrameKeyDisplay::DayOnly) {
                doc.write(1 + i + 1,
                          1,
                          summarizedData[i].timeFrameKey.date(),
                          make_border(make_border(QXlsx::Format(),
                                                  QXlsx::Format::BorderThin,
                                                  {false, false, true, false}),
                                      QXlsx::Format::BorderMedium,
                                      {false, false, false, true}));
            } else {
                doc.write(1 + i + 1,
                          1,
                          summarizedData[i].timeFrameKey.toString("yyyy-MM"),
                          make_border(make_border(QXlsx::Format(),
                                                  QXlsx::Format::BorderThin,
                                                  {false, false, true, true}),
                                      QXlsx::Format::BorderMedium,
                                      {false, false, false, true}));
            }
            doc.write(1 + i + 1,
                      2,
                      static_cast<double>(summarizedData[i].seconds) / (3600.0 * 24),
                      make_border(timeEntryFormat,
                                  QXlsx::Format::BorderThin,
                                  {false, false, true, false}));
            doc.write(1 + i + 1,
                      3,
                      static_cast<double>(summarizedData[i].seconds) / 3600.0,
                      make_border(decimalFormat,
                                  QXlsx::Format::BorderThin,
                                  {false, false, true, false}));
            doc.write(1 + i + 1,
                      4,
                      summarizedData[i].groupingKey,
                      make_border(make_border(QXlsx::Format(),
                                              QXlsx::Format::BorderThin,
                                              {false, false, true, false}),
                                  QXlsx::Format::BorderMedium,
                                  {false, true, false, false}));
        }

        // Write footer

        doc.write(1 + summarizedData.count() + 1,
                  1,
                  QVariant(),
                  make_border(QXlsx::Format(),
                              QXlsx::Format::BorderMedium,
                              {true, false, false, true}));
        doc.write(1 + summarizedData.count() + 1,
                  2,
                  QVariant(),
                  make_border(QXlsx::Format(),
                              QXlsx::Format::BorderMedium,
                              {true, false, false, false}));
        doc.write(1 + summarizedData.count() + 1,
                  3,
                  QVariant(),
                  make_border(QXlsx::Format(),
                              QXlsx::Format::BorderMedium,
                              {true, false, false, false}));
        doc.write(1 + summarizedData.count() + 1,
                  4,
                  QVariant(),
                  make_border(QXlsx::Format(),
                              QXlsx::Format::BorderMedium,
                              {true, true, false, false}));

        doc.write(1 + summarizedData.count() + 2,
                  1,
                  QVariant(),
                  make_border(QXlsx::Format(),
                              QXlsx::Format::BorderMedium,
                              {false, false, false, true}));
        doc.write(1 + summarizedData.count() + 2,
                  2,
                  QString("=+SUM(B2:B%1)").arg(1 + summarizedData.count()),
                  timeEntryBoldFormat);
        doc.write(1 + summarizedData.count() + 2,
                  3,
                  QString("=+SUM(C2:C%1)").arg(1 + summarizedData.count()),
                  decimalBoldFormat);
        doc.write(1 + summarizedData.count() + 2,
                  4,
                  QVariant(),
                  make_border(QXlsx::Format(),
                              QXlsx::Format::BorderMedium,
                              {false, true, false, false}));

        doc.write(1 + summarizedData.count() + 3,
                  1,
                  QVariant(),
                  make_border(QXlsx::Format(),
                              QXlsx::Format::BorderMedium,
                              {false, false, true, true}));
        doc.write(1 + summarizedData.count() + 3,
                  2,
                  QVariant(),
                  make_border(QXlsx::Format(),
                              QXlsx::Format::BorderMedium,
                              {false, false, true, false}));
        doc.write(1 + summarizedData.count() + 3,
                  3,
                  QString("=C%1/8").arg(1 + summarizedData.count() + 2),
                  make_border(decimalFormat,
                              QXlsx::Format::BorderMedium,
                              {false, false, true, false}));
        doc.write(1 + summarizedData.count() + 3,
                  4,
                  QVariant(),
                  make_border(QXlsx::Format(),
                              QXlsx::Format::BorderMedium,
                              {false, true, true, false}));

        doc.autosizeColumnWidth();

        if (!doc.saveAs(_fileName)) {
            qWarning() << "Error exporting file";
            emit finished(false, tr("Could not save file to provided file path."));
        } else {
            emit finished(true, QString());
        }
    } catch (const std::exception &e) {
        emit finished(false, tr("Error while creating file: %1").arg(e.what()));
    }
}

void ExcelBillingExporter::setTitle(const QString &newTitle)
{
    _title = newTitle;
}

void ExcelBillingExporter::setSubject(const QString &newSubject)
{
    _subject = newSubject;
}

void ExcelBillingExporter::setCreator(const QString &newCreator)
{
    _creator = newCreator;
}

void ExcelBillingExporter::setCompany(const QString &newCompany)
{
    _company = newCompany;
}

void ExcelBillingExporter::setDescription(const QString &newDescription)
{
    _description = newDescription;
}
