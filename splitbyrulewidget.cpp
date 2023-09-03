#include "splitbyrulewidget.hpp"
#include "ui_splitbyrulewidget.h"

#include "sortablelistlayout.hpp"

SplitByRuleWidget::SplitByRuleWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SplitByRuleWidget)
{
    ui->setupUi(this);
    auto retain_hidden = [=](QWidget *widget) {
        QSizePolicy sp = widget->sizePolicy();
        sp.setRetainSizeWhenHidden(true);
        widget->setSizePolicy(sp);
    };
    connect(ui->upButton, &QPushButton::clicked, this, &SplitByRuleWidget::moveUp);
    connect(ui->downButton, &QPushButton::clicked, this, &SplitByRuleWidget::moveDown);
    connect(ui->removeButton, &QPushButton::clicked, this, &SplitByRuleWidget::remove);

    retain_hidden(ui->upButton);
    retain_hidden(ui->downButton);
    retain_hidden(ui->removeButton);
}

SplitByRuleWidget::~SplitByRuleWidget()
{
    delete ui;
}

BillingSummarizer::SplitByRule SplitByRuleWidget::getRule() const
{
    return BillingSummarizer::SplitByRule{
        .ruleColumn = static_cast<BillingSummarizer::SplitByRule::Column>(
            ui->columnComboBox->currentIndex()),
        .ruleOperator = static_cast<BillingSummarizer::SplitByRule::Operator>(
            ui->operatorComboBox->currentIndex()),
        .ruleComparisonValue = ui->comparisonValueEdit->text()};
}

void SplitByRuleWidget::setRule(const BillingSummarizer::SplitByRule &rule)
{
    ui->columnComboBox->setCurrentIndex(static_cast<int>(rule.ruleColumn));
    ui->operatorComboBox->setCurrentIndex(static_cast<int>(rule.ruleOperator));
    ui->comparisonValueEdit->setText(rule.ruleComparisonValue);
}

void SplitByRuleWidget::layoutOrderUpdated()
{
    if (parentWidget() && parentWidget()->layout()) {
        const auto layout = dynamic_cast<SortableListLayout *>(parentWidget()->layout());
        if (layout) {
            int index = layout->indexOf(this);
            ui->upButton->setHidden(index == 0);
            ui->downButton->setHidden(index == layout->count() - 1);
        }
    }
}
