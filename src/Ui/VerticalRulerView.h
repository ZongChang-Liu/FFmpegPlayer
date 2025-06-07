//
// Created by liu_zongchang on 25-6-6 下午3:23.
// Email 1439797751@qq.com
//

#ifndef HEIGHT_SCALE_VIEW_H
#define HEIGHT_SCALE_VIEW_H

#include <QWidget>

#include "Def.h"
#include "ElaTheme.h"

class VerticalRulerView final : public QWidget {
    Q_OBJECT
public:
    explicit  VerticalRulerView(QWidget *parent = nullptr);
    ~VerticalRulerView() override = default;

    void initRuler(int minValue, int maxValue, int curValue);
    void setFontSize(int fontSize);
    void setPointerSize(int pointerSize);

    Q_SLOT void setThemeMode(ElaThemeType::ThemeMode themeMode);

    Q_SIGNALS:
    Q_SIGNAL void sigValueChanged(int value);
protected:
    void resizeEvent(QResizeEvent* event) override;
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
private:
    ElaThemeType::ThemeMode m_themeMode = ElaThemeType::Light;
    int m_pointerSize{10};
    int m_fontSize{10};

    QRect m_backgroundRect;
    QRect m_cursorRect;

    bool m_isPressed{false};
    bool m_isHover{false};
    QPoint m_mousePos;

    int m_curValue{0};
    int m_minValue{0};
    int m_maxValue{100};
};



#endif //HEIGHT_SCALE_VIEW_H
