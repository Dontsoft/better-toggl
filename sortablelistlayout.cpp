#include "sortablelistlayout.hpp"

#include <QDebug>

std::tuple<int, int> SortableListLayout::moveWidgetUp(QWidget *widget)
{
    int index = indexOf(widget);
    int formerIndex = index;
    if (index > 0) {
        index -= 1;
    }
    removeWidget(widget);
    insertWidget(index, widget);
    emit orderChanged();
    return {formerIndex, index};
}

std::tuple<int, int> SortableListLayout::moveWidgetDown(QWidget *widget)
{
    int index = indexOf(widget);
    int formerIndex = index;
    if (index < count() - 1) {
        index += 1;
    }
    removeWidget(widget);
    insertWidget(index, widget);
    emit orderChanged();
    return {formerIndex, index};
}
