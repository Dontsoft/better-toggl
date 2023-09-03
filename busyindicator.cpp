#include "busyindicator.hpp"
#include <QPainter>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>

#include "global.hpp"

constexpr int SIZE = 32;
constexpr int ANIMATION_DURATION = 2000;

BusyIndicator::BusyIndicator(QWidget *parent)
    : QWidget{parent}
    , _animationGroup(new QParallelAnimationGroup(this))
{
    connect(this, &BusyIndicator::startAngleChanged, this, [=](int) { update(); });
    connect(this, &BusyIndicator::spanAngleChanged, this, [=](int) { update(); });

    auto startAngleAnimation = new QPropertyAnimation(this, "startAngle");
    auto spanAngleAnimation = new QPropertyAnimation(this, "spanAngle");

    startAngleAnimation->setStartValue(0);
    startAngleAnimation->setKeyValueAt(0.25, 0);
    startAngleAnimation->setKeyValueAt(0.5, 0);
    startAngleAnimation->setKeyValueAt(0.75, 90);
    startAngleAnimation->setEndValue(360);
    startAngleAnimation->setEasingCurve(QEasingCurve(QEasingCurve::InOutExpo));
    startAngleAnimation->setDuration(ANIMATION_DURATION);
    startAngleAnimation->setLoopCount(-1);

    spanAngleAnimation->setStartValue(10);
    spanAngleAnimation->setKeyValueAt(0.25, 270);
    spanAngleAnimation->setKeyValueAt(0.5, 360);
    spanAngleAnimation->setKeyValueAt(0.75, 270);
    spanAngleAnimation->setEndValue(10);
    spanAngleAnimation->setEasingCurve(QEasingCurve(QEasingCurve::InOutExpo));
    spanAngleAnimation->setDuration(ANIMATION_DURATION);
    spanAngleAnimation->setLoopCount(-1);

    _animationGroup->addAnimation(startAngleAnimation);
    _animationGroup->addAnimation(spanAngleAnimation);
    _animationGroup->start(QAbstractAnimation::KeepWhenStopped);
}

void BusyIndicator::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setPen(QPen(better_toggl::PRIMARY_COLOR, 4));
    const auto widgetRect = rect();
    auto busyRect = QRectF(0,
                           0,
                           qMin(widgetRect.width(), SIZE) - 6,
                           qMin(widgetRect.height(), SIZE) - 6);
    busyRect.moveCenter(widgetRect.center());
    p.drawArc(busyRect, (-_startAngle + 90) * 16, (-_spanAngle) * 16);
}
