#include "lighttheme.hpp"

#include <QGuiApplication>

LightTheme::LightTheme()
    : Theme{}
{}

QColor LightTheme::background1() const
{
    return QColor("#f5f5f5");
}
QColor LightTheme::foreground1() const
{
    return QColor(Qt::white);
}
QColor LightTheme::foreground2() const
{
    return QColor("#fafafa");
}
QColor LightTheme::text() const
{
    return QColor("#171717");
}
QColor LightTheme::placeholder() const
{
    auto t = text();
    t.setAlphaF(0.6);
    return t;
}
QColor LightTheme::primary() const
{
    return Theme::PRIMARY_COLOR;
}
QColor LightTheme::primaryAlternate() const
{
    return Theme::PRIMARY_COLOR_ALT;
}
QColor LightTheme::error() const
{
    return Theme::ERROR_COLOR;
}
QColor LightTheme::errorAlternate() const
{
    return Theme::ERROR_COLOR_ALT;
}
QColor LightTheme::textOnPrimary() const
{
    return QColor(Qt::white);
}
QColor LightTheme::shadow() const
{
    return QColor("#262626");
}
QColor LightTheme::light() const
{
    return QColor("#e5e5e5");
}
QColor LightTheme::midlight() const
{
    return QColor("#d4d4d4");
}
QColor LightTheme::button() const
{
    return QColor("#a3a3a3");
}
QColor LightTheme::mid() const
{
    return QColor("#737373");
}
QColor LightTheme::dark() const
{
    return QColor("#525252");
}
QColor LightTheme::buttonText() const
{
    return text();
}

QColor LightTheme::background1Disabled() const
{
    return QColor("#d4d4d4");
}
QColor LightTheme::foreground1Disabled() const
{
    return QColor("#f5f5f5");
}
QColor LightTheme::foreground2Disabled() const
{
    return QColor("#d4d4d4");
}
QColor LightTheme::textDisabled() const
{
    return QColor("#404040");
}
QColor LightTheme::placeholderDisabled() const
{
    auto t = textDisabled();
    t.setAlphaF(0.6);
    return t;
}
QColor LightTheme::primaryDisabled() const
{
    return Theme::PRIMARY_COLOR_DISABLED;
}
QColor LightTheme::primaryAltDisabled() const
{
    return Theme::PRIMARY_COLOR_ALT_DISABLED;
}
QColor LightTheme::errorDisabled() const
{
    return Theme::ERROR_COLOR_DISABLED;
}
QColor LightTheme::errorAltDisabled() const
{
    return Theme::ERROR_COLOR_ALT_DISABLED;
}
QColor LightTheme::textOnPrimaryDisabled() const
{
    return QColor("#e5e5e5");
}
QColor LightTheme::shadowDisabled() const
{
    return QColor("#262626");
}
QColor LightTheme::lightDisabled() const
{
    return QColor("#d4d4d4");
}
QColor LightTheme::midlightDisabled() const
{
    return QColor("#a3a3a3");
}
QColor LightTheme::buttonDisabled() const
{
    return QColor("#737373");
}
QColor LightTheme::midDisabled() const
{
    return QColor("#525252");
}
QColor LightTheme::darkDisabled() const
{
    return QColor("#262626");
}
QColor LightTheme::buttonTextDisabled() const
{
    return textDisabled();
}

QPalette LightTheme::palette() const
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
QString LightTheme::stylesheet() const
{
    auto _stylesheet = getBaseStylesheet();
    return _stylesheet;
}
