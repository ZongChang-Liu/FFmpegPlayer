//
// Created by liu_zongchang on 2024/11/26 3:52.
// Email 1439797751@qq.com
//
//

#include <QPainter>
#include "StackedWidget.h"

#include <QLayout>
#include <QPropertyAnimation>
#include <QMouseEvent>
#include <QPainterPath>

#include "ElaTheme.h"

ExpandButton::ExpandButton(QWidget* parent) : QWidget(parent)
{
    setFixedSize(30,70);
    m_themeMode = eTheme->getThemeMode();
    connect(eTheme, &ElaTheme::themeModeChanged, this, &ExpandButton::onThemeModeChanged);
}

ExpandButton::~ExpandButton() = default;

void ExpandButton::doAnimationExpandCollapse()
{
    auto* animation = new QPropertyAnimation(this, "m_angle");
    connect(animation, &QPropertyAnimation::valueChanged, this, [=](const QVariant&) {
        update();
    });
    animation->setEasingCurve(QEasingCurve::InOutCubic);
    animation->setStartValue(m_angle);
    animation->setDuration(500);
    animation->setEndValue(m_angle == 0 ? -180 : 0);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void ExpandButton::setAngle(const int angle)
{
    m_angle = angle;
    update();
}

int ExpandButton::getAngle() const
{
    return m_angle;
}

void ExpandButton::onThemeModeChanged(ElaThemeType::ThemeMode themeMode)
{
    m_themeMode = themeMode;
    update();
}

void ExpandButton::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(ElaThemeColor(m_themeMode, BasicBaseLine), 1.5));
    painter.setBrush(ElaThemeColor(m_themeMode, WindowCentralStackBase));

    QRect targetRect = this->rect();
    targetRect.adjust(1, 1, 6, -1);
    painter.drawRoundedRect(targetRect, 5, 5);

    auto iconFont = QFont("ElaAwesome");
    iconFont.setPixelSize(13);
    painter.setFont(iconFont);
    painter.setPen(isEnabled() ? ElaThemeColor(m_themeMode, BasicText) : ElaThemeColor(m_themeMode, BasicTextDisable));
    const QRectF expandIconRect(0, 0, width(), height());
    painter.translate(this->width() / 2, this->height() / 2);
    painter.rotate(m_angle);
    painter.translate(-this->width() / 2, -this->height() / 2);
    painter.drawText(expandIconRect, Qt::AlignCenter, QChar(static_cast<unsigned short>(ElaIconType::ChevronsRight)));

    painter.restore();
}

void ExpandButton::mousePressEvent(QMouseEvent* event)
{
    m_isPressed = true;
    update();
    QWidget::mousePressEvent(event);
}

void ExpandButton::mouseReleaseEvent(QMouseEvent* event)
{
    QWidget::mouseReleaseEvent(event);
    m_isPressed = false;
    m_isExpanded = !m_isExpanded;
    Q_EMIT sigExpandButtonClicked();
}

void ExpandButton::mouseMoveEvent(QMouseEvent* event)
{
    update();
    QWidget::mouseMoveEvent(event);
}

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
void ExpandButton::enterEvent(QEnterEvent* event)
{
    QWidget::enterEvent(event);
#else
void ExpandButton::enterEvent(QEvent* event)
{
    QWidget::enterEvent(event);
#endif
    this->setCursor(Qt::PointingHandCursor);
    this->update();
}

void ExpandButton::leaveEvent(QEvent* event)
{
    QWidget::leaveEvent(event);
    this->setCursor(Qt::ArrowCursor);
    this->update();
}

StackedWidget::StackedWidget(QWidget *parent) : QStackedWidget(parent) {
    this->layout()->setAlignment(Qt::AlignRight);
    setMouseTracking(true);
    m_themeMode = eTheme->getThemeMode();
    m_expandButton = new ExpandButton(this);
    m_expandButton->setVisible(false);
    m_expandedWidth = this->width();
    m_expandButton->move(0, (this->height() - m_expandButton->height() ) / 2);

    connect(m_expandButton, &ExpandButton::sigExpandButtonClicked, this, &StackedWidget::onExpandButtonClicked);
    connect(eTheme, &ElaTheme::themeModeChanged, this, &StackedWidget::onThemeModeChanged);
}

StackedWidget::~StackedWidget() = default;

void StackedWidget::setHasRadius(const bool isHasRadius)
{
    m_isHasRadius = isHasRadius;
    update();
}

void StackedWidget::setIsTransparent(const bool isTransparent)
{
    m_isTransparent = isTransparent;
    update();
}

void StackedWidget::enableExpandCollapse(const bool isEnable) const
{
    m_expandButton->setVisible(isEnable);
}

void StackedWidget::setExpandedWidth(const int width)
{
    m_expandedWidth = width;
}

void StackedWidget::onThemeModeChanged(const ElaThemeType::ThemeMode themeMode) {
    m_themeMode = themeMode;
}

void StackedWidget::onExpandButtonClicked()
{
    doAnimationExpandCollapse();
    raise();
}

void StackedWidget::doAnimationExpandCollapse()
{
    m_isExpanded = !m_isExpanded;
    auto* animation = new QPropertyAnimation(this, "maximumWidth");
    animation->setEasingCurve(QEasingCurve::InOutCubic);
    animation->setStartValue(this->width());
    animation->setDuration(285);
    animation->setEndValue(m_isExpanded ? m_expandedWidth : m_expandButton->width());
    connect(animation, &QPropertyAnimation::valueChanged, this, [=](const QVariant& value) {
            setFixedWidth(value.toInt());
    });
    animation->start(QAbstractAnimation::DeleteWhenStopped);
    m_expandButton->doAnimationExpandCollapse();
}

bool StackedWidget::isExpanded() const
{
    return m_isExpanded;
}

void StackedWidget::paintEvent(QPaintEvent* event)
{
    if (!m_isTransparent) {
        QPainter painter(this);
        painter.save();
        painter.setRenderHint(QPainter::Antialiasing);

        painter.setPen(QPen(ElaThemeColor(m_themeMode, BasicBaseLine), 1.5));
        painter.setBrush(ElaThemeColor(m_themeMode, WindowCentralStackBase));

        if (m_isHasRadius)
        {
            QRect targetRect = this->rect();
            targetRect.adjust(1, 1, 10, -10);
            painter.drawRoundedRect(targetRect, 10, 10);
        }
        else
        {

            if (m_expandButton->isVisible())
            {
                QPainterPath path;
                path.moveTo(m_expandButton->width(),0);
                path.lineTo(m_expandButton->width(), (this->height() - m_expandButton->height()) / 2);
                path.moveTo(m_expandButton->width(), this->height() / 2 + m_expandButton->height() / 2);
                path.lineTo(m_expandButton->width(), this->height());
                path.lineTo(this->width() - 1, this->height());
                path.lineTo(this->width() - 1, 0);
                path.lineTo(m_expandButton->width() - 1, 0);
                painter.drawPath(path);
            }
            else
            {

                painter.drawRect(this->rect().adjusted(1, 1, -1, -1));
            }
        }
        painter.restore();
    }
    QStackedWidget::paintEvent(event);
}

void StackedWidget::resizeEvent(QResizeEvent* event)
{
    QStackedWidget::resizeEvent(event);
    m_expandButton->move(0, (this->height() - m_expandButton->height() ) / 2);
}

