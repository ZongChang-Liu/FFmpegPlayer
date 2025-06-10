//
// Created by liu_zongchang on 25-6-6 下午3:23.
// Email 1439797751@qq.com
//

#include "VerticalRulerView.h"
#include <QPainter>
#include <QDebug>
#include <QMouseEvent>
VerticalRulerView::VerticalRulerView(QWidget* parent) : QWidget(parent)
{
    this->setMouseTracking(true);
    m_backgroundRect = QRect(m_pointerSize, m_pointerSize + 1,
                               width() - 2 - m_pointerSize,
                               height() - 2 - 2 * m_pointerSize);
    m_cursorRect = QRect(m_pointerSize, m_pointerSize + 2,
                               width() - 3 - m_pointerSize,
                               0);
    m_mousePos = QPoint(m_backgroundRect.left() + m_backgroundRect.width() / 2, m_backgroundRect.top() + m_backgroundRect.height() - 1);
    update();
}

void VerticalRulerView::setValue(const int value)
{
    if (value < m_minValue || value > m_maxValue)
    {
        qWarning() << "Value out of range: " << value << "min:" << m_minValue << "max:" << m_maxValue;
        return;
    }
    if (m_curValue == value)
    {
        return;
    }

    double ratio = static_cast<double>(m_curValue - m_minValue) / (m_maxValue - m_minValue);
    ratio = qBound(0.0, ratio, 1.0);
    const double cursorRectY = m_backgroundRect.top() + (1.0 - ratio) * m_backgroundRect.height();
    m_cursorRect.setHeight(cursorRectY - m_backgroundRect.top() - 2);
    m_mousePos.setY(cursorRectY);

    Q_EMIT sigValueChanged(m_curValue);
    update();
}

void VerticalRulerView::setRange(const int min, const int max)
{
    m_minValue = min;
    m_maxValue = max;
    if (m_curValue < m_minValue || m_curValue > m_maxValue)
    {
        m_curValue = qBound(m_minValue, m_curValue, m_maxValue);
    }

    double ratio = static_cast<double>(m_curValue - m_minValue) / (m_maxValue - m_minValue);
    ratio = qBound(0.0, ratio, 1.0);
    const double cursorRectY = m_backgroundRect.top() + (1.0 - ratio) * m_backgroundRect.height();
    m_cursorRect.setHeight(cursorRectY - m_backgroundRect.top() - 2);
    m_mousePos.setY(cursorRectY);
    update();
}

void VerticalRulerView::initRuler(const int minValue, const int maxValue, const int curValue)
{
    m_minValue = minValue;
    m_maxValue = maxValue;
    m_curValue = curValue;

    Q_EMIT sigValueChanged(m_curValue);
    update();
}

void VerticalRulerView::setFontSize(const int fontSize)
{
    m_fontSize = fontSize;
    update();
}

void VerticalRulerView::setPointerSize(const int pointerSize)
{
    m_pointerSize = pointerSize;
    resizeEvent(nullptr);
}

void VerticalRulerView::setThemeMode(const ElaThemeType::ThemeMode themeMode)
{
    m_themeMode = themeMode;
    update();
}

void VerticalRulerView::resizeEvent(QResizeEvent* event)
{
    m_backgroundRect = QRect(m_pointerSize, m_pointerSize + 1,
                               width() - 2 - m_pointerSize,
                               height() - 2 - 2 * m_pointerSize);

    double ratio = static_cast<double>(m_curValue - m_minValue) / (m_maxValue - m_minValue);
    ratio = qBound(0.0, ratio, 1.0);
    const double cursorRectY = m_backgroundRect.top() + (1.0 - ratio) * m_backgroundRect.height();
    m_cursorRect = QRect(m_pointerSize, m_pointerSize + 2,
                               width() - 3 - m_pointerSize,
                               cursorRectY - m_backgroundRect.top() - 2);
    m_mousePos.setY(cursorRectY);

    QWidget::resizeEvent(event);
}

void VerticalRulerView::paintEvent(QPaintEvent* event)
{
    QWidget::paintEvent(event);
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing, true);


    // Draw the control rectangle
    if (m_isHover)
    {
        painter.setPen(QPen(ElaThemeColor(m_themeMode, BasicBorderHover), 1));
    }
    else
    {
        painter.setPen(QPen(ElaThemeColor(m_themeMode, BasicBorder), 1));
    }
    painter.setBrush(ElaThemeColor(m_themeMode, BasicBaseDeep));
    painter.drawRect(m_backgroundRect);

    //Draw the background
    painter.setPen(Qt::NoPen);
    painter.setBrush(ElaThemeColor(m_themeMode, BasicBase));
    painter.drawRect(m_cursorRect);


    // Draw the Scale
    QFont font;
    font.setFamily("Microsoft YaHei");
    font.setPixelSize(m_fontSize);

    painter.setFont(font);
    if (m_isHover)
    {
        painter.setPen(QPen(ElaThemeColor(m_themeMode, BasicText), 1));
    } else
    {

        painter.setPen(QPen(ElaThemeColor(m_themeMode, BasicTextNoFocus), 1));
    }
    double startY = m_backgroundRect.top();
    const double m_tickMarkStep = 1.0 * m_backgroundRect.height() / 100;
    int num = 100;
    while (startY <  m_backgroundRect.top() + m_backgroundRect.height())
    {
        if (num % 10 == 0 && num != 100 && num != 0)
        {
            painter.drawLine(m_pointerSize + 1, startY, m_pointerSize + 16, startY);
            painter.drawText(m_pointerSize + 20, startY + 4, QString::number(num));
        }
        else if (num % 5 == 0)
        {
            painter.drawLine(m_pointerSize + 1, startY, m_pointerSize + 11, startY);
        }
        else
        {
            painter.drawLine(m_pointerSize + 1, startY, m_pointerSize + 6, startY);
        }
        startY += m_tickMarkStep;
        num--;
    }
    painter.drawLine(m_pointerSize, m_backgroundRect.top(), m_pointerSize, m_backgroundRect.bottom());
    painter.drawLine(m_pointerSize + 1, m_backgroundRect.top(), m_pointerSize + 16, m_backgroundRect.top());
    painter.drawText(m_pointerSize + 20, m_backgroundRect.top() + 13, "100%");
    painter.drawLine(m_pointerSize + 1, m_backgroundRect.bottom(), m_pointerSize + 16, m_backgroundRect.bottom());
    painter.drawText(m_pointerSize + 20, m_backgroundRect.bottom() - 3, "0");



    // Draw the pointer
    painter.setPen(QPen(ElaThemeColor(m_themeMode, StatusDanger), 1.5));
    painter.setBrush(ElaThemeColor(m_themeMode, StatusDanger));
    QPolygon triangle;
    triangle << QPoint(0 , m_cursorRect.bottom() + 2 - m_pointerSize / 2)
             << QPoint(0 , m_cursorRect.bottom() + 2  + m_pointerSize / 2)
             << QPoint(m_pointerSize, m_cursorRect.bottom() + 2);
    painter.drawPolygon(triangle);
    painter.drawLine(m_pointerSize, m_cursorRect.bottom() + 2,
                        m_pointerSize + m_backgroundRect.width() - 2, m_cursorRect.bottom() + 2);
}

void VerticalRulerView::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_isPressed = true;
        if (event->pos().y() < m_backgroundRect.top())
        {
            m_mousePos.setY(m_backgroundRect.top());
        }
        else if (event->pos().y() > m_backgroundRect.bottom())
        {
            m_mousePos.setY(m_backgroundRect.bottom());
        }
        else
        {
            m_mousePos = event->pos();
        }

        m_cursorRect.setHeight(m_mousePos.y() - m_backgroundRect.top() - 2);
        m_curValue = static_cast<int>(m_maxValue - (m_maxValue - m_minValue) * (1.0 * (m_mousePos.y() - m_backgroundRect.top()) / m_backgroundRect.height()));
        Q_EMIT sigValueChanged(m_curValue);
        update();
    }

    QWidget::mousePressEvent(event);
}

void VerticalRulerView::mouseReleaseEvent(QMouseEvent* event)
{
    m_isPressed = false;
    QWidget::mouseReleaseEvent(event);
}

void VerticalRulerView::mouseMoveEvent(QMouseEvent* event)
{
    m_isHover = m_backgroundRect.contains(event->pos());
    if (m_isHover)
    {
        m_mousePos = event->pos();
    }

    if (m_isPressed)
    {
        m_cursorRect.setHeight(m_mousePos.y() - m_backgroundRect.top() - 2);
        m_curValue = static_cast<int>(m_maxValue - (m_maxValue - m_minValue) * (1.0 * (m_mousePos.y() - m_backgroundRect.top()) / m_backgroundRect.height()));
        Q_EMIT sigValueChanged(m_curValue);
    }

    update();
    QWidget::mouseMoveEvent(event);
}

void VerticalRulerView::leaveEvent(QEvent* event)
{
    double ratio = static_cast<double>(m_curValue - m_minValue) / (m_maxValue - m_minValue);
    ratio = qBound(0.0, ratio, 1.0);
    const double cursorRectY = m_backgroundRect.top() + (1.0 - ratio) * m_backgroundRect.height();
    if (m_mousePos.y() != cursorRectY)
    {
        m_mousePos.setY(cursorRectY);
        update();
    }
    QWidget::leaveEvent(event);
}

void VerticalRulerView::wheelEvent(QWheelEvent* event)
{
    m_curValue = event->angleDelta().y() > 0 ? m_curValue + 1 : m_curValue - 1;
    if (m_curValue < m_minValue)
    {
        m_curValue = m_minValue;
    }
    else if (m_curValue > m_maxValue)
    {
        m_curValue = m_maxValue;
    }

    double ratio = static_cast<double>(m_curValue - m_minValue) / (m_maxValue - m_minValue);
    ratio = qBound(0.0, ratio, 1.0);
    const double cursorRectY = m_backgroundRect.top() + (1.0 - ratio) * m_backgroundRect.height();
    m_cursorRect.setHeight(cursorRectY - m_backgroundRect.top() - 2);
    update();
    QWidget::wheelEvent(event);
    event->accept();
}
