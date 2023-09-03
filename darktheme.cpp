#include "darktheme.hpp"

#include <QGuiApplication>
DarkTheme::DarkTheme()
    : Theme{}

{}

QColor DarkTheme::background1() const
{
    return QColor("#171717");
}
QColor DarkTheme::foreground1() const
{
    return QColor("#404040");
}
QColor DarkTheme::foreground2() const
{
    return QColor("#262626");
}
QColor DarkTheme::text() const
{
    return QColor(Qt::white);
}
QColor DarkTheme::placeholder() const
{
    auto t = text();
    t.setAlphaF(0.6);
    return t;
}
QColor DarkTheme::primary() const
{
    return Theme::PRIMARY_COLOR;
}
QColor DarkTheme::primaryAlternate() const
{
    return Theme::PRIMARY_COLOR_ALT;
}
QColor DarkTheme::error() const
{
    return Theme::ERROR_COLOR;
}
QColor DarkTheme::errorAlternate() const
{
    return Theme::ERROR_COLOR_ALT;
}
QColor DarkTheme::textOnPrimary() const
{
    return QColor(Qt::white);
}
QColor DarkTheme::shadow() const
{
    return QColor("#262626");
}
QColor DarkTheme::light() const
{
    return QColor("#a3a3a3");
}
QColor DarkTheme::midlight() const
{
    return QColor("#737373");
}
QColor DarkTheme::button() const
{
    return QColor("#525252");
}
QColor DarkTheme::mid() const
{
    return QColor("#404040");
}
QColor DarkTheme::dark() const
{
    return QColor("#171717");
}
QColor DarkTheme::buttonText() const
{
    return text();
}

QColor DarkTheme::background1Disabled() const
{
    return QColor("#171717");
}
QColor DarkTheme::foreground1Disabled() const
{
    return QColor("#525252");
}
QColor DarkTheme::foreground2Disabled() const
{
    return QColor("#404040");
}
QColor DarkTheme::textDisabled() const
{
    return QColor("#e5e5e5");
}
QColor DarkTheme::placeholderDisabled() const
{
    auto t = textDisabled();
    t.setAlphaF(0.6);
    return t;
}
QColor DarkTheme::primaryDisabled() const
{
    return Theme::PRIMARY_COLOR_DISABLED;
}
QColor DarkTheme::primaryAltDisabled() const
{
    return Theme::PRIMARY_COLOR_ALT_DISABLED;
}
QColor DarkTheme::errorDisabled() const
{
    return Theme::ERROR_COLOR_DISABLED;
}
QColor DarkTheme::errorAltDisabled() const
{
    return Theme::ERROR_COLOR_ALT_DISABLED;
}
QColor DarkTheme::textOnPrimaryDisabled() const
{
    return QColor("#e5e5e5");
}
QColor DarkTheme::shadowDisabled() const
{
    return QColor("#262626");
}
QColor DarkTheme::lightDisabled() const
{
    return QColor("#d4d4d4");
}
QColor DarkTheme::midlightDisabled() const
{
    return QColor("#a3a3a3");
}
QColor DarkTheme::buttonDisabled() const
{
    return QColor("#737373");
}
QColor DarkTheme::midDisabled() const
{
    return QColor("#525252");
}
QColor DarkTheme::darkDisabled() const
{
    return QColor("#262626");
}
QColor DarkTheme::buttonTextDisabled() const
{
    return textDisabled();
}
QPalette DarkTheme::palette() const
{
    // https://doc.qt.io/qt-6/qpalette.html
    QPalette _palette = QGuiApplication::palette();

    _palette.setColor(QPalette::Active, QPalette::Window, background1());
    _palette.setColor(QPalette::Active, QPalette::WindowText, text());
    _palette.setColor(QPalette::Active, QPalette::Base, foreground1());
    _palette.setColor(QPalette::Active, QPalette::AlternateBase, foreground2());
    _palette.setColor(QPalette::Active, QPalette::ToolTipBase, primary());
    _palette.setColor(QPalette::Active, QPalette::ToolTipText, textOnPrimary());
    _palette.setColor(QPalette::Active, QPalette::PlaceholderText, placeholder());
    _palette.setColor(QPalette::Active, QPalette::Text, text());
    _palette.setColor(QPalette::Active, QPalette::Button, button());
    _palette.setColor(QPalette::Active, QPalette::ButtonText, buttonText());
    _palette.setColor(QPalette::Active, QPalette::BrightText, text());
    _palette.setColor(QPalette::Active, QPalette::Light, light());
    _palette.setColor(QPalette::Active, QPalette::Midlight, midlight());
    _palette.setColor(QPalette::Active, QPalette::Dark, dark());
    _palette.setColor(QPalette::Active, QPalette::Mid, mid());
    _palette.setColor(QPalette::Active, QPalette::Shadow, shadow());
    _palette.setColor(QPalette::Active, QPalette::Highlight, primary());
    _palette.setColor(QPalette::Active, QPalette::HighlightedText, textOnPrimary());
    _palette.setColor(QPalette::Active, QPalette::Link, primary());
    _palette.setColor(QPalette::Active, QPalette::LinkVisited, primaryAlternate());

    _palette.setColor(QPalette::Disabled, QPalette::Window, background1Disabled());
    _palette.setColor(QPalette::Disabled, QPalette::WindowText, textDisabled());
    _palette.setColor(QPalette::Disabled, QPalette::Base, foreground1Disabled());
    _palette.setColor(QPalette::Disabled, QPalette::AlternateBase, foreground2Disabled());
    _palette.setColor(QPalette::Disabled, QPalette::ToolTipBase, primaryDisabled());
    _palette.setColor(QPalette::Disabled, QPalette::ToolTipText, textOnPrimaryDisabled());
    _palette.setColor(QPalette::Disabled, QPalette::PlaceholderText, placeholderDisabled());
    _palette.setColor(QPalette::Disabled, QPalette::Text, textDisabled());
    _palette.setColor(QPalette::Disabled, QPalette::Button, buttonDisabled());
    _palette.setColor(QPalette::Disabled, QPalette::ButtonText, buttonTextDisabled());
    _palette.setColor(QPalette::Disabled, QPalette::BrightText, textDisabled());
    _palette.setColor(QPalette::Disabled, QPalette::Light, lightDisabled());
    _palette.setColor(QPalette::Disabled, QPalette::Midlight, midlightDisabled());
    _palette.setColor(QPalette::Disabled, QPalette::Dark, darkDisabled());
    _palette.setColor(QPalette::Disabled, QPalette::Mid, midDisabled());
    _palette.setColor(QPalette::Disabled, QPalette::Shadow, shadowDisabled());
    _palette.setColor(QPalette::Disabled, QPalette::Highlight, primaryDisabled());
    _palette.setColor(QPalette::Disabled, QPalette::HighlightedText, textOnPrimaryDisabled());
    _palette.setColor(QPalette::Disabled, QPalette::Link, primaryDisabled());
    _palette.setColor(QPalette::Disabled, QPalette::LinkVisited, primaryAltDisabled());

    _palette.setColor(QPalette::Inactive, QPalette::Window, background1());
    _palette.setColor(QPalette::Inactive, QPalette::WindowText, textDisabled());
    _palette.setColor(QPalette::Inactive, QPalette::Base, foreground1());
    _palette.setColor(QPalette::Inactive, QPalette::AlternateBase, foreground2());
    _palette.setColor(QPalette::Inactive, QPalette::ToolTipBase, primaryDisabled());
    _palette.setColor(QPalette::Inactive, QPalette::ToolTipText, textOnPrimaryDisabled());
    _palette.setColor(QPalette::Inactive, QPalette::PlaceholderText, placeholderDisabled());
    _palette.setColor(QPalette::Inactive, QPalette::Text, textDisabled());
    _palette.setColor(QPalette::Inactive, QPalette::Button, buttonDisabled());
    _palette.setColor(QPalette::Inactive, QPalette::ButtonText, buttonTextDisabled());
    _palette.setColor(QPalette::Inactive, QPalette::BrightText, textDisabled());
    _palette.setColor(QPalette::Inactive, QPalette::Light, lightDisabled());
    _palette.setColor(QPalette::Inactive, QPalette::Midlight, midlightDisabled());
    _palette.setColor(QPalette::Inactive, QPalette::Dark, darkDisabled());
    _palette.setColor(QPalette::Inactive, QPalette::Mid, midDisabled());
    _palette.setColor(QPalette::Inactive, QPalette::Shadow, shadowDisabled());
    _palette.setColor(QPalette::Inactive, QPalette::Highlight, primaryDisabled());
    _palette.setColor(QPalette::Inactive, QPalette::HighlightedText, textOnPrimaryDisabled());
    _palette.setColor(QPalette::Inactive, QPalette::Link, primaryDisabled());
    _palette.setColor(QPalette::Inactive, QPalette::LinkVisited, primaryAltDisabled());

    return _palette;
}

QString DarkTheme::stylesheet() const
{
    auto _stylesheet = getBaseStylesheet();
    return _stylesheet;
}
