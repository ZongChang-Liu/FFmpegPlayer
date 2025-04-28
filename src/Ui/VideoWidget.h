//
// Created by liu_zongchang on 2025/1/13 3:00.
// Email 1439797751@qq.com
// 
//

#ifndef VIDEO_WIDGET_H
#define VIDEO_WIDGET_H

#include <QMutex>
#include <QWidget>
extern "C" {
    #include <libavutil/frame.h>
}

class VideoWidget final : public QWidget {
    Q_OBJECT
public:
    explicit VideoWidget(QWidget *parent = nullptr);
    ~VideoWidget() override;
    Q_SLOT void onUpdateFrame(const AVFrame* frame);
    Q_SLOT void onUpdateImage(const QImage& image);
    void setRecordFlag(bool flag);
    int getImage(QImage &image) const;
protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent* event) override;
private:
    bool m_recordFlag{false};
    bool m_showRecordFlag{false};

    QImage m_image;
    int m_margin = 12;
    int m_radius = 8;
    int m_showWidth = 0;
    int m_showHeight = 0;
    QRect m_showRect;

    qint64 m_lastFrameTime{0};
    int m_frameCount{0};
    double m_frameRate{0};
};




#endif //VIDEO_WIDGET_H
