#ifndef SORTABLELISTLAYOUT_HPP
#define SORTABLELISTLAYOUT_HPP

#include <QVBoxLayout>

class SortableListLayout : public QVBoxLayout
{
    Q_OBJECT
public:
    using QVBoxLayout::QVBoxLayout;

    std::tuple<int, int> moveWidgetUp(QWidget *widget);
    std::tuple<int, int> moveWidgetDown(QWidget *widget);

signals:
    void orderChanged();
};

#endif // SORTABLELISTLAYOUT_HPP
