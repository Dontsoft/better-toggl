#ifndef BUSYINDICATOR_HPP
#define BUSYINDICATOR_HPP

#include <QWidget>

class QParallelAnimationGroup;

class BusyIndicator : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int startAngle MEMBER _startAngle READ getStartAngle WRITE setStartAngle NOTIFY
                   startAngleChanged)
    Q_PROPERTY(int spanAngle MEMBER _spanAngle READ getSpanAngle WRITE setSpanAngle NOTIFY
                   spanAngleChanged)
public:
    explicit BusyIndicator(QWidget *parent = nullptr);

    void setStartAngle(int angle)
    {
        _startAngle = angle;
        emit startAngleChanged(_startAngle);
    }

    void setSpanAngle(int angle)
    {
        _spanAngle = angle;
        emit spanAngleChanged(_spanAngle);
    }

    int getStartAngle() const { return _startAngle; }
    int getSpanAngle() const { return _spanAngle; }

protected:
    void paintEvent(QPaintEvent *event);

signals:
    void startAngleChanged(int angle);
    void spanAngleChanged(int angle);

private:
    int _startAngle = 0;
    int _spanAngle = 0;
    QParallelAnimationGroup *_animationGroup;
};

#endif // BUSYINDICATOR_HPP
