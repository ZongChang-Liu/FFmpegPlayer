//
// Created by liu_zongchang on 25-6-7 下午12:37.
// Email 1439797751@qq.com
//

#ifndef SWING_ARM_VIEW_H
#define SWING_ARM_VIEW_H

#include <QWidget>

#include "Def.h"

class SwingArmView final : public QWidget{
    Q_OBJECT
public:
    explicit SwingArmView(QWidget *parent = nullptr);
    ~SwingArmView() override = default;

    void isDrawText(bool draw);
    void setAngleRange(int min, int max);

    Q_SLOT void setAngle(double angle);
    Q_SIGNAL void sigAngleChanged(double angle);

    Q_SLOT void setThemeMode(ElaThemeType::ThemeMode mode);
protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent *event) override;
    void leaveEvent(QEvent* event) override;
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

    void drawCircle(QPainter &painter, int radius) const;
    void drawScale(QPainter &painter, int radius) const;
    void drawPointer(QPainter &painter, int radius) const;
    void drawText(QPainter &painter) const;
private:

    ElaThemeType::ThemeMode m_themeMode{ElaThemeType::Light};
    bool m_drawText{false};

    int m_margin{1};
    int m_fontSize{12};

    int m_radius{0};
    int m_circleWidth{2};
    QPoint m_center;
    QRect m_contentRect;

    int m_angle{0};
    int m_angleMax{60};
    int m_angleMin{-60};

    double m_currentAngle = 0;
    double m_angularRange{120};


    bool m_isHover{false};
    bool m_isPressed{false};
};



#endif //SWING_ARM_VIEW_H
