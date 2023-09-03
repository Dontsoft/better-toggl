#ifndef SPLITBYRULEWIDGET_HPP
#define SPLITBYRULEWIDGET_HPP

#include <QWidget>

#include "billingsummarizer.hpp"

namespace Ui {
    class SplitByRuleWidget;
}

class SplitByRuleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SplitByRuleWidget(QWidget *parent = nullptr);
    ~SplitByRuleWidget();

    BillingSummarizer::SplitByRule getRule() const;
    void setRule(const BillingSummarizer::SplitByRule &rule);

public slots:
    void layoutOrderUpdated();

signals:
    void moveUp();
    void moveDown();
    void remove();

private:
    Ui::SplitByRuleWidget *ui;
};

#endif // SPLITBYRULEWIDGET_HPP
