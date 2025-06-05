//
// Created by liu_zongchang on 2025/1/13 3:00.
// Email 1439797751@qq.com
// 
//

#ifndef VIDEO_WIDGET_H
#define VIDEO_WIDGET_H

#include <QWidget>
extern "C" {
#include <libavutil/frame.h>
}
class VideoView;

class VideoWidget final : public QWidget {
    Q_OBJECT
public:
    explicit VideoWidget(QWidget *parent = nullptr);
    ~VideoWidget() override;

    int getImage(QImage &image) const;

    void setRecordFlag(bool flag) const;
    void setLiveFlag(bool flag) const;
    void setPlayFlag(bool flag) const;

    // Q_SLOT void onUpdateImage(const QImage& image) const;
    Q_SLOT void onUpdateFrame(const AVFrame* frame) const;
    Q_SLOT void onAspectRatioChanged(double aspectRatio);
protected:
    void resizeEvent(QResizeEvent* event) override;
private:
    VideoView *m_videoView{nullptr};
    double m_aspectRatio{1920.0 / 1080.0};

    qint64 m_lastFrameTime{0};
    int m_frameCount{0};
    double m_frameRate{0};
};




#endif //VIDEO_WIDGET_H
