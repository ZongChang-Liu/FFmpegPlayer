//
// Created by liu_zongchang on 25-6-5 下午8:02.
// Email 1439797751@qq.com
//

#ifndef VIDEO_VIEW_H
#define VIDEO_VIEW_H


#include <QWidget>

class VideoView final : public QWidget
{
    Q_OBJECT
public:
    explicit  VideoView(QWidget *parent = nullptr);
    ~VideoView() override = default;

    [[nodiscard]] QImage getImage() const;

    Q_SLOT void onUpdateImage(const QImage& image);
    void setRecordFlag(bool flag);
    void setLiveFlag(bool flag);
    void setPlayFlag(bool flag);
    void setRoundFlag(bool flag);
    void setShadowFlag(bool flag);

    [[nodiscard]] bool getShadowFlag() const;
    [[nodiscard]] int getRadius() const;
    [[nodiscard]] int getMargin() const;


    Q_SIGNALS:
    Q_SIGNAL void sigAspectRatioChanged(double aspectRatio);
protected:
    void paintEvent(QPaintEvent *event) override;
private:
    QImage m_image;

    int m_radius = 8;
    int m_margin = 12;

    bool m_recordFlag{false};
    bool m_liveFlag{false};
    bool m_playFlag{false};

    bool m_isRound{false};
    bool m_isShadow{false};

    bool m_showLiveFlag{false};
    bool m_showRecordFlag{false};

    QTimer* m_updateTimer{nullptr};
    double m_scaleFactor{1.0};
    double m_aspectRatio{1920.0 / 1080.0};


    qint64 m_lastFrameTime{0};
    int m_frameCount{0};
    double m_frameRate{0};
};


#endif //VIDEO_VIEW_H
