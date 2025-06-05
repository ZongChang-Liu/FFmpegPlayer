//
// Created by liu_zongchang on 2025/1/13 3:00.
// Email 1439797751@qq.com
// 
//


#include "VideoWidget.h"

#include <QVBoxLayout>
#include <QDebug>
#include <QDateTime>

#include "VideoView.h"

VideoWidget::VideoWidget(QWidget *parent) : QWidget(parent) {
    this->setMinimumSize(300, 400);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->setAlignment(Qt::AlignCenter);
    this->setLayout(layout);

    m_videoView = new VideoView(this);
    m_videoView->setObjectName("VideoView");
    m_videoView->setRoundFlag(true);
    m_videoView->setShadowFlag(true);
    layout->addWidget(m_videoView);

    connect(m_videoView, &VideoView::sigAspectRatioChanged, this, &VideoWidget::onAspectRatioChanged);
}

VideoWidget::~VideoWidget() = default;

int VideoWidget::getImage(QImage& image) const
{
    if (m_videoView == nullptr) {
        return -1;
    }

    const QImage img = m_videoView->getImage();
    if (img.isNull()) {
        return -1;
    }
    image = img.copy();
    return 0;
}

void VideoWidget::setRecordFlag(const bool flag) const
{
     m_videoView->setRecordFlag(flag);
}

void VideoWidget::setLiveFlag(const bool flag) const
{
    m_videoView->setLiveFlag(flag);
}

void VideoWidget::setPlayFlag(const bool flag) const
{
    m_videoView->setPlayFlag(flag);
}

// void VideoWidget::onUpdateImage(const QImage& image) const
// {
//     m_videoView->onUpdateImage(image);
// }

void VideoWidget::onUpdateFrame(const AVFrame* frame) const
{
    if (frame == nullptr) {
        return;
    }
    const QImage m_image = QImage(frame->data[0], frame->width, frame->height, frame->linesize[0], QImage::Format_RGB888).copy();
    m_videoView->onUpdateImage(m_image);
}

void VideoWidget::onAspectRatioChanged(const double aspectRatio)
{
    m_aspectRatio = aspectRatio;
    resizeEvent(nullptr);
}

void VideoWidget::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    if (m_videoView) {
        int width = this->width();
        int height = static_cast<int>(width / m_aspectRatio);
        if (height > this->height()) {
            height = this->height();
            width = static_cast<int>(height * m_aspectRatio);
        }
        m_videoView->setFixedSize(width, height);
    }
}