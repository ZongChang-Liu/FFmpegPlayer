//
// Created by liu_zongchang on 25-6-5 下午8:02.
// Email 1439797751@qq.com
//

#include "VideoView.h"

#include <QTimer>
#include <QDateTime>
#include <QPainter>
#include <QPainterPath>

VideoView::VideoView(QWidget* parent) : QWidget(parent) {
    this->setMinimumSize(300, 400);
    m_updateTimer = new QTimer(this);
    m_updateTimer->setInterval(500);
    connect(m_updateTimer, &QTimer::timeout, this, [this]() {
        m_showRecordFlag = !m_showRecordFlag;
        m_showLiveFlag = !m_showLiveFlag;
        update();
    });
}


QImage VideoView::getImage() const
{
    return m_image.copy();
}

void VideoView::onUpdateImage(const QImage& image)
{
    if (image.isNull())
    {
        return;
    }
    if (const double aspectRatio = static_cast<double>(image.width()) / image.height(); aspectRatio != m_aspectRatio) {
        m_aspectRatio = aspectRatio;
        Q_EMIT sigAspectRatioChanged(m_aspectRatio);
    }
    m_image = image;
    if (m_lastFrameTime == 0) {
        m_lastFrameTime = QDateTime::currentMSecsSinceEpoch();
    }
    if (const qint64 currentTime = QDateTime::currentMSecsSinceEpoch(); currentTime - m_lastFrameTime >= 1000) {
        m_frameRate = m_frameCount * 1000.0 / (currentTime - m_lastFrameTime);
        m_frameCount = 0;
        m_lastFrameTime = currentTime;
    }
    m_frameCount++;
    update();
}

void VideoView::setRecordFlag(const bool flag)
{
    m_recordFlag = flag;
    update();
    if (m_recordFlag || m_liveFlag && !m_updateTimer->isActive()) {
        m_updateTimer->start();
    } else if (!m_recordFlag && !m_liveFlag && m_updateTimer->isActive()) {
        m_updateTimer->stop();
    }
}

void VideoView::setLiveFlag(const bool flag)
{
    m_liveFlag = flag;
    update();
    if (m_recordFlag || m_liveFlag && !m_updateTimer->isActive()) {
        m_updateTimer->start();
    } else if (!m_recordFlag && !m_liveFlag && m_updateTimer->isActive()) {
        m_updateTimer->stop();
    }
}

void VideoView::setPlayFlag(const bool flag)
{
    m_playFlag = flag;
    if (!m_playFlag) {
        if (!m_image.isNull()) {
            const QImage grayImage = m_image.convertToFormat(QImage::Format_Grayscale8);
            m_image = grayImage;
        }
    }
    update();
}

void VideoView::setRoundFlag(const bool flag)
{
    m_isRound = flag;
}

void VideoView::setShadowFlag(const bool flag)
{
    m_isShadow = flag;
}

bool VideoView::getShadowFlag() const
{
    return m_isShadow;
}

int VideoView::getRadius() const
{
    return m_radius;
}

int VideoView::getMargin() const
{
    return m_margin;
}

void VideoView::paintEvent(QPaintEvent* event)
{
    if (m_image.isNull()) {
        return;
    }

    QPainter painter(this);

    painter.setPen(Qt::NoPen);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);

    painter.setBrush(Qt::transparent);
    painter.drawRect(this->rect());

    if (m_isShadow)
    {
        QPainterPath effectShadowPath;
        effectShadowPath.setFillRule(Qt::WindingFill);
        auto color = QColor(50, 50, 50);
        for (int i = 0; i < m_margin; i++) {

            if (m_isRound)
            {
                effectShadowPath.addRoundedRect(
                         m_margin - i,
                         m_margin - i,
                        this->width() - (m_margin - i) * 2,
                        this->height() - (m_margin - i) * 2,
                        m_radius + i, m_radius + i);
            } else
            {
                effectShadowPath.addRect(
                        m_margin - i,
                        m_margin - i,
                        this->width() - (m_margin - i) * 2,
                        this->height() - (m_margin - i) * 2);
            }
            const int alpha = 1 * (m_margin - i + 1);
            color.setAlpha(alpha > 255 ? 255 : alpha);
            painter.setPen(color);
            painter.drawPath(effectShadowPath);
        }
    }



    painter.setPen(Qt::NoPen);
    QRect imageRect;
    if (!m_isShadow)
    {
       imageRect =  QRect(0,0,this->width(),this->height());
    } else
    {
        imageRect = QRect(m_margin, m_margin, this->width() - m_margin * 2, this->height() - m_margin * 2);
    }

    if (m_isRound)
    {
        QPainterPath pixmapPath;
        pixmapPath.addRoundedRect(imageRect, m_radius, m_radius);
        painter.setClipPath(pixmapPath);
        painter.drawPixmap(imageRect, QPixmap::fromImage(m_image), m_image.rect());
    } else
    {
        painter.drawPixmap(imageRect, QPixmap::fromImage(m_image), m_image.rect());
    }
    QFont font;
    font.setFamily("Microsoft YaHei");
    font.setPixelSize(20);
    font.setBold(true);

    if (m_recordFlag) {
        painter.setPen(Qt::red);
        painter.setBrush(Qt::red);

        painter.setFont(font);
        const int high = QFontMetrics(font).height();
        painter.drawText(imageRect.x() + 35,imageRect.y() + high + 4,QString("REC"));
        if (m_showRecordFlag)
        {
            painter.drawEllipse(imageRect.x() + 15, imageRect.y() + 15, 15, 15);
        }

    }

    //绘制fps在左下角
    painter.setPen(Qt::white);
    painter.setBrush(Qt::white);
    painter.setFont(font);
    const int high = QFontMetrics(font).height();
    painter.drawText(imageRect.x() + 35, imageRect.y() + high + 20,
                     QString("FPS: %1").arg(m_frameRate, 0, 'f', 2));

    if (m_liveFlag)
    {
        painter.setPen(Qt::green);
        painter.setBrush(Qt::green);
        painter.setFont(font);
        const int high = QFontMetrics(font).height();
        painter.drawText(imageRect.x() + 35,
                         imageRect.y() + high + 34,
                         QString("Live"));
        if (m_showLiveFlag)
        {
            painter.drawEllipse(imageRect.x() + 15, imageRect.y() + 45, 15, 15);
        }
    }

    if (m_recordFlag)
    {
        painter.setPen(QPen(Qt::red, 5));
        painter.setBrush(Qt::NoBrush);
        if (m_isRound)
        {
            painter.drawRoundedRect(imageRect.x(),imageRect.y(),
                    imageRect.width(),
                    imageRect.height(),
                    m_radius, m_radius);
        } else
        {
            painter.drawRect(imageRect);
        }
    } else if (m_liveFlag)
    {
        painter.setPen(QPen(Qt::green, 5));
        painter.setBrush(Qt::NoBrush);
        if (m_isRound)
        {
            painter.drawRoundedRect(imageRect.x(),imageRect.y(),
                    imageRect.width(),
                    imageRect.height(),
                    m_radius, m_radius);
        } else
        {
            painter.drawRect(imageRect);
        }
    }
}
