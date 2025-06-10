//
// Created by liu_zongchang on 25-6-7 下午12:37.
// Email 1439797751@qq.com
//

#include "SwingArmView.h"
#include <QResizeEvent>
#include <QDebug>
#include <qmath.h>
#include <QPainter>
#include <QPainterPath>

#include "ElaTheme.h"

SwingArmView::SwingArmView(QWidget* parent) : QWidget(parent)
{
    this->setMouseTracking(true);
    m_radius = this->width() / 2 - m_margin;
    m_center = QPoint(m_margin + m_radius, m_margin + m_radius);
    m_contentRect = QRect(m_center.x() - m_radius, m_center.y() - m_radius, m_radius * 2, m_radius);
}

void SwingArmView::isDrawText(const bool draw)
{
    m_drawText = draw;
    update();
}

void SwingArmView::setAngleRange(const int min, const int max)
{
    if (min - max >= 190)
    {
        qWarning() << "Invalid angle range: min" << min << "max" << max;
        return;
    }
    m_angleMin = min;
    m_angleMax = max;
    m_angularRange = m_angleMax - m_angleMin;
}

void SwingArmView::setAngle(const double angle)
{
    if (m_currentAngle > m_angularRange / 2)
    {
        m_currentAngle = m_angularRange / 2;
    }
    else if (m_currentAngle < -m_angularRange / 2)
    {
        m_currentAngle = -m_angularRange / 2;
    }

    if (qFuzzyCompare(m_currentAngle, angle))
    {
        return;
    }
    m_currentAngle = angle;

    Q_EMIT sigAngleChanged(m_currentAngle);
    update();
}

void SwingArmView::setThemeMode(const ElaThemeType::ThemeMode mode)
{
    m_themeMode = mode;
    update();
}

void SwingArmView::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_isPressed = true;
        if (m_contentRect.contains(event->pos()))
        {
            const QPoint pos = event->pos();
            const double angle = qAtan2(pos.y() - m_center.y(), pos.x() - m_center.x()) * 180 / M_PI;
            m_currentAngle = qRound(angle * 10) / 10.0 + 90; // Convert to degrees and adjust to start from the top
            Q_EMIT sigAngleChanged(m_currentAngle);
        }

        update();
    }
    QWidget::mousePressEvent(event);
}

void SwingArmView::mouseReleaseEvent(QMouseEvent* event)
{
    m_isPressed = false;
    update();
    QWidget::mouseReleaseEvent(event);
}

void SwingArmView::mouseMoveEvent(QMouseEvent* event)
{
    m_isHover = m_contentRect.contains(event->pos());
    update();
    QWidget::mouseMoveEvent(event);
}

void SwingArmView::wheelEvent(QWheelEvent* event)
{
    QWidget::wheelEvent(event);
    m_currentAngle += event->angleDelta().y() > 0 ? 1 : -1;
    m_currentAngle = qRound(m_currentAngle);
    if (m_currentAngle > m_angularRange / 2)
    {
        m_currentAngle = m_angularRange / 2;
    }
    else if (m_currentAngle < -m_angularRange / 2)
    {
        m_currentAngle = -m_angularRange / 2;
    }
    Q_EMIT sigAngleChanged(m_currentAngle);

    update();
    event->accept();
}

void SwingArmView::leaveEvent(QEvent* event)
{
    m_isHover = false;
    m_isPressed = false;
    update();
    QWidget::leaveEvent(event);
}

void SwingArmView::resizeEvent(QResizeEvent* event)
{
    m_radius = this->width() / 2 - m_margin;
    m_center = QPoint(m_margin + m_radius, m_margin + m_radius);
    m_contentRect = QRect(m_center.x() - m_radius, m_center.y() - m_radius, m_radius * 2, m_radius);
    QWidget::resizeEvent(event);
}

void SwingArmView::paintEvent(QPaintEvent* event)
{
    QWidget::paintEvent(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.translate(m_center);
    drawCircle(painter, m_radius);
    drawPointer(painter, m_radius - m_circleWidth - 25);
    drawScale(painter, m_radius - m_circleWidth - 1);
    if (m_drawText)
    {
        drawText(painter);
    }
}

void SwingArmView::drawCircle(QPainter& painter, const int radius) const
{
    QRadialGradient gradient;
    gradient.setCenter(m_center);
    gradient.setFocalPoint(m_center);
    gradient.setRadius(radius);


    painter.setPen(ElaThemeColor(m_themeMode, BasicBorder));
    painter.setBrush(ElaThemeColor(m_themeMode, BasicBorder));
    if (m_isHover)
    {
        painter.setPen(ElaThemeColor(m_themeMode, BasicBorderHover));
        painter.setBrush(ElaThemeColor(m_themeMode, BasicBorderHover));
    }

    painter.save();
    QPainterPath outRing;
    outRing.arcTo(-radius, -radius, 2 * radius, 2 * radius, m_angleMin + 90, m_angleMax - m_angleMin);
    QPainterPath inRing;
    outRing.moveTo(0, 0);
    inRing.moveTo(0, 0);
    inRing.addEllipse(-radius + m_circleWidth, -radius + m_circleWidth, 2 * (radius - m_circleWidth),
                      2 * (radius - m_circleWidth));
    outRing.closeSubpath();
    painter.drawPath(outRing.subtracted(inRing));
    painter.restore();
}

void SwingArmView::drawScale(QPainter& painter, int radius) const
{
    QFont font;
    font.setFamily("Microsoft YaHei");
    font.setPixelSize(m_fontSize - 3);


    for (int i = 0; i < m_angularRange + 1; ++i)
    {
        int scaleLength;
        if (i % 20 == 0)
        {
            scaleLength = 15;
        }
        else if (i % 10 == 0)
        {
            scaleLength = 7;
        }
        else if (i % 2 == 0)
        {
            scaleLength = 4;
        }
        else
        {
            continue;
        }
        if (const double absValue = abs(m_angularRange / 2 - i - m_currentAngle); absValue < 0.3)
        {
            painter.setPen(QPen(ElaThemeColor(m_themeMode, StatusDanger),2));
        }
        else if (m_isHover)
        {
            painter.setPen(ElaThemeColor(m_themeMode, BasicText));
        }
        else
        {
            painter.setPen(ElaThemeColor(m_themeMode, BasicTextNoFocus));
        }

        QPointF point(0, 0);
        painter.save();
        point.setX(radius * qCos((90 - m_angularRange / 2 + i) * M_PI / 180));
        point.setY(radius * qSin((90 - m_angularRange / 2 + i) * M_PI / 180));
        painter.translate(point.x(), -point.y());
        painter.rotate(90 - (90 - m_angularRange / 2) - i);
        painter.drawLine(0, 0, 0, scaleLength);
        if (i % 20 == 0)
        {
            painter.setFont(font);
            QFontMetrics fm(painter.font());
            const int textWidth = fm.horizontalAdvance(QString::number(m_angularRange / 2 - i));
            painter.drawText(-textWidth / 2, scaleLength + 5, textWidth, fm.height(), Qt::AlignCenter,
                             QString::number(m_angularRange / 2 - i));
        }
        painter.restore();
    }
}

void SwingArmView::drawPointer(QPainter& painter, int radius) const
{
    painter.setPen(Qt::NoPen);
    QPainterPath pointPath;
    pointPath.moveTo(5, 0);
    pointPath.lineTo(0, -radius);
    pointPath.lineTo(-5, 0);
    pointPath.arcTo(-5, 0, 10, 10, 180, 180);
    QPainterPath inRing;
    inRing.addEllipse(QPointF(0, 0), 10, 10);
    painter.save();
    painter.rotate(m_currentAngle);
    if (m_isPressed)
    {
        painter.setBrush(ElaThemeColor(m_themeMode, PrimaryPress));
    }
    else if (m_isHover)
    {
        painter.setBrush(ElaThemeColor(m_themeMode, PrimaryHover));
    }
    else
    {
        painter.setBrush(ElaThemeColor(m_themeMode, PrimaryNormal));
    }
    painter.drawPath(pointPath.subtracted(inRing));
    painter.restore();
}

void SwingArmView::drawText(QPainter& painter) const
{
    QFont font;
    font.setFamily("Microsoft YaHei");
    font.setPixelSize(m_fontSize);
    painter.save();

    if (m_isHover)
    {
        painter.setPen(ElaThemeColor(m_themeMode, BasicText));
    }
    else
    {
        painter.setPen(ElaThemeColor(m_themeMode, BasicTextNoFocus));
    }
    painter.setFont(font);
    const QString text = "角度：" + QString::number(m_angle) + "°";
    const int textWidth = painter.fontMetrics().horizontalAdvance(text);
    const int textHeight = painter.fontMetrics().height();
    painter.drawText(-textWidth / 2, +20, textWidth, textHeight, Qt::AlignCenter, text);
    painter.restore();
}
