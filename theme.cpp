#include "theme.hpp"

#include <QComboBox>
#include <QDebug>
#include <QGuiApplication>
#include <QPainter>
#include <QPushButton>
#include <QStyleOptionButton>
#include <QWidget>

const QColor Theme::PRIMARY_COLOR = QColor("#4f46e5");
const QColor Theme::PRIMARY_COLOR_ALT = QColor("#4338ca");
const QColor Theme::PRIMARY_COLOR_DISABLED = QColor("#818cf8");
const QColor Theme::PRIMARY_COLOR_ALT_DISABLED = QColor("#818cf8");
const QColor Theme::ERROR_COLOR = QColor("#dc2626");
const QColor Theme::ERROR_COLOR_ALT = QColor("#b91c1c");
const QColor Theme::ERROR_COLOR_DISABLED = QColor("#ef4444");
const QColor Theme::ERROR_COLOR_ALT_DISABLED = QColor("#ef4444");

Theme::Theme() {}

QStyle *Theme::style()
{
    return new ThemedStyle(this);
}

QString Theme::getBaseStylesheet() const
{
    return QString("QLineEdit, QSpinBox, QDoubleSpinBox {min-height: 24px; padding: 4px;}"
                   "QLabel[errorLabel=true]{background-color: %1; color: %2; padding: 8px;}")
        .arg(error().name(), textOnPrimary().name());
}

ThemedStyle::ThemedStyle(Theme *theme)
    : QProxyStyle{}
    , _theme(theme)
{}

QPalette ThemedStyle::standardPalette() const
{
    return _theme->palette();
}

void ThemedStyle::polish(QWidget *widget)
{
    if (qobject_cast<QPushButton *>(widget) || qobject_cast<QComboBox *>(widget))
        widget->setAttribute(Qt::WA_Hover, true);

    if (qobject_cast<QPushButton *>(widget)) {
        widget->setCursor(QCursor(Qt::PointingHandCursor));
    }
}

void ThemedStyle::unpolish(QWidget *widget)
{
    if (qobject_cast<QPushButton *>(widget) || qobject_cast<QComboBox *>(widget))
        widget->setAttribute(Qt::WA_Hover, false);
    if (qobject_cast<QPushButton *>(widget)) {
        widget->unsetCursor();
    }
}

void ThemedStyle::drawControl(ControlElement element,
                              const QStyleOption *option,
                              QPainter *painter,
                              const QWidget *widget) const
{
    switch (element) {
    case ControlElement::CE_PushButtonBevel: {
        const QStyleOptionButton *buttonOption = qstyleoption_cast<const QStyleOptionButton *>(
            option);
        const QVariant isPrimaryVariant = widget->property("--primary");
        const QVariant isErrorVariant = widget->property("--error");
        const bool isPrimaryButton = (isPrimaryVariant.isValid() && !isPrimaryVariant.isNull()
                                      && qvariant_cast<bool>(isPrimaryVariant))
                                     || buttonOption
                                            && (buttonOption->features
                                                & QStyleOptionButton::DefaultButton);
        const bool isErrorButton = isErrorVariant.isValid() && !isErrorVariant.isNull()
                                   && qvariant_cast<bool>(isErrorVariant);

        const bool isHover = (buttonOption && (buttonOption->state & QStyle::State_MouseOver));
        const bool isFlat = buttonOption && (buttonOption->features & QStyleOptionButton::Flat);

        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);

        if (!isFlat) {
            if (isErrorButton) {
                if (isHover) {
                    painter->setBrush(_theme->errorAlternate());
                } else {
                    painter->setBrush(_theme->error());
                }
            } else if (isPrimaryButton) {
                if (isHover) {
                    painter->setBrush(_theme->primaryAlternate());
                } else {
                    painter->setBrush(_theme->primary());
                }
            } else {
                if (isHover) {
                    painter->setBrush(_theme->button());
                } else {
                    painter->setBrush(_theme->mid());
                }
            }
        } else {
            painter->setBrush(Qt::transparent);
        }
        painter->setPen(QPen(Qt::transparent));
        auto buttonRect = QRect(buttonOption->rect.left(),
                                buttonOption->rect.top(),
                                buttonOption->rect.width(),
                                24);
        buttonRect.moveCenter(buttonOption->rect.center());
        painter->drawRoundedRect(buttonRect, 4, 4);
        // QProxyStyle::drawControl(element, option, painter, widget);
        painter->restore();
        break;
    }
    case ControlElement::CE_PushButtonLabel: {
        const QStyleOptionButton *buttonOption = qstyleoption_cast<const QStyleOptionButton *>(
            option);
        const QVariant isPrimaryVariant = widget->property("--primary");
        const QVariant isErrorVariant = widget->property("--error");
        const bool isPrimaryButton = (isPrimaryVariant.isValid() && !isPrimaryVariant.isNull()
                                      && qvariant_cast<bool>(isPrimaryVariant))
                                     || buttonOption
                                            && (buttonOption->features
                                                & QStyleOptionButton::DefaultButton);
        const bool isErrorButton = isErrorVariant.isValid() && !isErrorVariant.isNull()
                                   && qvariant_cast<bool>(isErrorVariant);
        const bool isFlat = buttonOption && (buttonOption->features & QStyleOptionButton::Flat);
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);
        if (isErrorButton) {
            if (isFlat) {
                painter->setPen(QPen(_theme->error()));
            } else {
                painter->setPen(QPen(_theme->textOnPrimary()));
            }
        } else if (isPrimaryButton) {
            if (isFlat) {
                painter->setPen(QPen(_theme->primary()));
            } else {
                painter->setPen(QPen(_theme->textOnPrimary()));
            }
        } else {
            if (isFlat) {
                painter->setPen(QPen(_theme->error()));
            } else {
                painter->setPen(QPen(_theme->textOnPrimary()));
            }
        }
        auto textRect = QRect(buttonOption->rect.left(),
                              buttonOption->rect.top(),
                              buttonOption->rect.width(),
                              24);

        textRect.moveCenter(buttonOption->rect.center());
        textRect.adjust(4, 0, -4, 0);

        QTextOption opt(Qt::AlignHCenter | Qt::AlignVCenter);
        painter->drawText(textRect,
                          buttonOption->fontMetrics.elidedText(buttonOption->text,
                                                               Qt::ElideRight,
                                                               textRect.width()),
                          opt);

        painter->restore();
        break;
    }
    default:
        QProxyStyle::drawControl(element, option, painter, widget);
        break;
    }
}

void ThemedStyle::drawPrimitive(PrimitiveElement element,
                                const QStyleOption *option,
                                QPainter *painter,
                                const QWidget *widget) const
{
    switch (element) {
    default:
        QProxyStyle::drawPrimitive(element, option, painter, widget);
        break;
    }
}
