#ifndef BILLINGSUMMARIZERWIDGET_HPP
#define BILLINGSUMMARIZERWIDGET_HPP

#include <QWidget>

#include "billingsummarizer.hpp"

namespace Ui {
    class BillingSummarizerWidget;
}

class BillingSummarizerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BillingSummarizerWidget(QWidget *parent = nullptr);
    ~BillingSummarizerWidget();

    BillingSummarizer getSummarizer() const;
    void setSummarizer(const BillingSummarizer &summarizer);

    void setUsers(const QList<toggl::ReportUser> &newUsers);

    void setTasks(const QList<toggl::Task> &newTasks);

private slots:
    void groupByComboBoxChanged(int index);
    void groupByTimespanComboBoxChanged(int index);
    void splitByComboBoxChanged(int index);
    void useWorkspaceRoundingChanged(int state);

    void addSplitByRule();

    void moveUp();
    void moveDown();
    void remove();

    void regenerateNamingForm();
    void switchPlacesInNamingForm(int formerIndex, int toIndex);

private:
    Ui::BillingSummarizerWidget *ui;

    QList<QPair<QString, QString>> getCurrentMapping() const;
    void setMapping(const QList<QPair<QString, QString>> &mapping);

    QList<toggl::Task> _tasks;
    QList<toggl::ReportUser> _users;

    BillingSummarizer::SplitBy _previousSplit = BillingSummarizer::SplitBy::DO_NOT_SPLIT;
    QMap<BillingSummarizer::SplitBy, QList<QPair<QString, QString>>> _groupNamingCache;
};

#endif // BILLINGSUMMARIZERWIDGET_HPP
