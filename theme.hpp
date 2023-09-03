#ifndef THEME_HPP
#define THEME_HPP

#include <QPalette>
#include <QProxyStyle>
#include <QString>

class Theme
{
public:
    static const QColor PRIMARY_COLOR;
    static const QColor PRIMARY_COLOR_ALT;
    static const QColor PRIMARY_COLOR_DISABLED;
    static const QColor PRIMARY_COLOR_ALT_DISABLED;
    static const QColor ERROR_COLOR;
    static const QColor ERROR_COLOR_ALT;
    static const QColor ERROR_COLOR_DISABLED;
    static const QColor ERROR_COLOR_ALT_DISABLED;

    Theme();
    virtual QString stylesheet() const = 0;
    virtual QPalette palette() const = 0;
    virtual QStyle *style();

    virtual QColor background1() const = 0;
    virtual QColor foreground1() const = 0;
    virtual QColor foreground2() const = 0;
    virtual QColor text() const = 0;
    virtual QColor placeholder() const = 0;
    virtual QColor primary() const = 0;
    virtual QColor primaryAlternate() const = 0;
    virtual QColor error() const = 0;
    virtual QColor errorAlternate() const = 0;
    virtual QColor textOnPrimary() const = 0;
    virtual QColor shadow() const = 0;
    virtual QColor light() const = 0;
    virtual QColor midlight() const = 0;
    virtual QColor button() const = 0;
    virtual QColor mid() const = 0;
    virtual QColor dark() const = 0;
    virtual QColor buttonText() const = 0;

    virtual QColor background1Disabled() const = 0;
    virtual QColor foreground1Disabled() const = 0;
    virtual QColor foreground2Disabled() const = 0;
    virtual QColor textDisabled() const = 0;
    virtual QColor placeholderDisabled() const = 0;
    virtual QColor primaryDisabled() const = 0;
    virtual QColor primaryAltDisabled() const = 0;
    virtual QColor errorDisabled() const = 0;
    virtual QColor errorAltDisabled() const = 0;
    virtual QColor textOnPrimaryDisabled() const = 0;
    virtual QColor shadowDisabled() const = 0;
    virtual QColor lightDisabled() const = 0;
    virtual QColor midlightDisabled() const = 0;
    virtual QColor buttonDisabled() const = 0;
    virtual QColor midDisabled() const = 0;
    virtual QColor darkDisabled() const = 0;
    virtual QColor buttonTextDisabled() const = 0;

protected:
    QString getBaseStylesheet() const;
};

class ThemedStyle : public QProxyStyle
{
public:
    ThemedStyle(Theme *theme);

    QPalette standardPalette() const override;

    void polish(QWidget *widget) override;
    void unpolish(QWidget *widget) override;

    void drawControl(ControlElement element,
                     const QStyleOption *option,
                     QPainter *painter,
                     const QWidget *widget) const override;

    void drawPrimitive(PrimitiveElement element,
                       const QStyleOption *option,
                       QPainter *painter,
                       const QWidget *widget) const override;

private:
    const Theme *_theme;
};

#endif // THEME_HPP
