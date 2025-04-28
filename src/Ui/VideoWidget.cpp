//
// Created by liu_zongchang on 2025/1/13 3:00.
// Email 1439797751@qq.com
// 
//

#include <QPainter>
#include <QDateTime>
#include <QPainterPath>
#include "VideoWidget.h"


VideoWidget::VideoWidget(QWidget *parent) : QWidget(parent) {
    this->setMinimumSize(300, 400);
}

VideoWidget::~VideoWidget() = default;

void VideoWidget::onUpdateFrame(const AVFrame* frame) {
    if (frame == nullptr) {
        return;
    }
    m_image = QImage(frame->data[0], frame->width, frame->height, frame->linesize[0], QImage::Format_RGB888).copy();
    if (m_lastFrameTime == 0) {
        m_lastFrameTime = QDateTime::currentMSecsSinceEpoch();
    }
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    if (currentTime - m_lastFrameTime >= 1000) {
        m_frameRate = m_frameCount * 1000.0 / (currentTime - m_lastFrameTime);
        m_frameCount = 0;
        m_lastFrameTime = currentTime;
        m_showRecordFlag = !m_showRecordFlag;
    }
    m_frameCount++;
    update();
}

void VideoWidget::onUpdateImage(const QImage& image)
{
    m_image = image;
    if (m_lastFrameTime == 0) {
        m_lastFrameTime = QDateTime::currentMSecsSinceEpoch();
    }
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    if (currentTime - m_lastFrameTime >= 1000) {
        m_frameRate = m_frameCount * 1000.0 / (currentTime - m_lastFrameTime);
        m_frameCount = 0;
        m_lastFrameTime = currentTime;
        m_showRecordFlag = !m_showRecordFlag;
    }
    m_frameCount++;
    update();
}

void VideoWidget::setRecordFlag(const bool flag)
{
    m_recordFlag = flag;
    update();
}

void VideoWidget::paintEvent(QPaintEvent *event) {
    if (m_image.isNull()) {
        return;
    }

    QPainter painter(this);

    painter.setPen(Qt::NoPen);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);

    painter.setBrush(Qt::transparent);
    painter.drawRect(this->rect());
    painter.setBrush(Qt::NoBrush);

    QPixmap pixmap(m_image.size());
    pixmap = pixmap.scaled(m_showWidth, m_showHeight, Qt::KeepAspectRatio);

    QPainterPath effectShadowPath;
    effectShadowPath.setFillRule(Qt::WindingFill);
    auto color = QColor(50, 50, 50);
    for (int i = 0; i < m_margin; i++) {
        effectShadowPath.addRoundedRect(
                (this->rect().width() - pixmap.width()) / 2 + m_margin - i,
                (this->rect().height() - pixmap.height()) / 2 + m_margin - i,
                pixmap.width() - (m_margin - i) * 2,
                pixmap.height() - (m_margin - i) * 2,
                m_radius + i, m_radius + i);
        const int alpha = 1 * (m_margin - i + 1);
        color.setAlpha(alpha > 255 ? 255 : alpha);
        painter.setPen(color);
        painter.drawPath(effectShadowPath);
    }

    painter.setPen(Qt::NoPen);
    const QRect foregroundRect(
            (this->rect().width() - pixmap.width()) / 2 + m_margin,
            (this->rect().height() - pixmap.height()) / 2 + m_margin,
            pixmap.width() - 2 * m_margin,
            pixmap.height() - 2 * m_margin);
    QPainterPath pixmapPath;
    pixmapPath.addRoundedRect(foregroundRect, m_radius, m_radius);
    painter.setClipPath(pixmapPath);
    painter.drawPixmap(foregroundRect, QPixmap::fromImage(m_image), m_image.rect());
    if (m_recordFlag) {
        painter.setPen(Qt::red);
        painter.setBrush(Qt::red);
        if (m_showRecordFlag)
        {
            painter.drawEllipse((this->rect().width() - pixmap.width()) / 2 + m_margin + 15,
                                (this->rect().height() - pixmap.height()) / 2 + m_margin + 15, 15, 15);

            //设置字体
            QFont font;
            font.setFamily("Microsoft YaHei");
            font.setPixelSize(20);
            font.setBold(true);
            painter.setFont(font);
            const int high = QFontMetrics(font).height();
            painter.drawText((this->rect().width() - pixmap.width()) / 2 + m_margin + 35,
                             (this->rect().height() - pixmap.height()) / 2 + m_margin + high + 4,
                             QString("REC"));
        }

        painter.setPen(QPen(Qt::red, 5));
        painter.setBrush(Qt::NoBrush);
        painter.drawRoundedRect(
                (this->rect().width() - pixmap.width()) / 2 + m_margin,
                (this->rect().height() - pixmap.height()) / 2 + m_margin,
                pixmap.width() - 2 * m_margin,
                pixmap.height() - 2 * m_margin,
                m_radius, m_radius);
    }
}

void VideoWidget::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    m_showWidth = this->rect().width() - m_margin;
    m_showHeight = this->rect().height() - m_margin;
    update();
}

int  VideoWidget::getImage(QImage &image) const {
    if(m_image.isNull()) {
       return -1;
    }
    image = m_image;
    return 0;
}
