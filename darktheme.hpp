#ifndef DARKTHEME_HPP
#define DARKTHEME_HPP

#include "theme.hpp"

class DarkTheme : public Theme
{
public:
    DarkTheme();
    QPalette palette() const override;
    QString stylesheet() const override;

    QColor background1() const override;
    QColor foreground1() const override;
    QColor foreground2() const override;
    ;
    QColor text() const override;
    QColor placeholder() const override;
    QColor primary() const override;
    QColor primaryAlternate() const override;
    QColor error() const override;
    QColor errorAlternate() const override;
    QColor textOnPrimary() const override;
    QColor shadow() const override;
    QColor light() const override;
    QColor midlight() const override;
    QColor button() const override;
    QColor mid() const override;
    QColor dark() const override;
    QColor buttonText() const override;

    QColor background1Disabled() const override;
    QColor foreground1Disabled() const override;
    QColor foreground2Disabled() const override;
    QColor textDisabled() const override;
    QColor placeholderDisabled() const override;
    QColor primaryDisabled() const override;
    QColor primaryAltDisabled() const override;
    QColor errorDisabled() const override;
    QColor errorAltDisabled() const override;
    QColor textOnPrimaryDisabled() const override;
    QColor shadowDisabled() const override;
    QColor lightDisabled() const override;
    QColor midlightDisabled() const override;
    QColor buttonDisabled() const override;
    QColor midDisabled() const override;
    QColor darkDisabled() const override;
    QColor buttonTextDisabled() const override;
};

#endif // DARKTHEME_HPP
