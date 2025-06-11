//
// Created by liu_zongchang on 2024/11/26 3:52.
// Email 1439797751@qq.com
//
//

#ifndef STACKED_WIDGET_H
#define STACKED_WIDGET_H


#include <QStackedWidget>

#include "Def.h"
#include "ElaScrollArea.h"

class ExpandButton final : public QWidget
{
    Q_PROPERTY(int m_angle READ getAngle WRITE setAngle)
    Q_OBJECT
public:
    explicit ExpandButton(QWidget *parent = nullptr);
    ~ExpandButton() override;

    void doAnimationExpandCollapse();

    void setAngle(int angle);
    [[nodiscard]] int getAngle() const;

    Q_SLOT void onThemeModeChanged(ElaThemeType::ThemeMode themeMode);

    Q_SIGNALS:
    Q_SIGNAL void sigExpandButtonClicked();
protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    void enterEvent(QEnterEvent* event) override;
#else
    void enterEvent(QEvent* event) override;
#endif
    void leaveEvent(QEvent* event) override;
private:
    ElaThemeType::ThemeMode m_themeMode;
    bool m_isPressed{false};
    bool m_isExpanded{true};
    int m_angle{0};
    int m_lineWidth{3};
};

class StackedWidget final : public QStackedWidget {
    Q_OBJECT
public:
    explicit StackedWidget(QWidget *parent = nullptr);
    ~StackedWidget() override;

    void setHasRadius(bool isHasRadius);
    void setIsTransparent(bool isTransparent);

    void enableExpandCollapse(bool isEnable) const;
    void setExpandedWidth(int width);

    void doAnimationExpandCollapse();

    bool isExpanded() const;

    Q_SLOT void onThemeModeChanged(ElaThemeType::ThemeMode themeMode);
    Q_SLOT void onExpandButtonClicked();
protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent* event) override;
private:
    ExpandButton* m_expandButton{nullptr};
    ElaScrollArea* m_scrollArea{nullptr};
    ElaThemeType::ThemeMode m_themeMode;
    bool m_isTransparent{false};
    bool m_isHasRadius{true};
    bool m_isExpanded{true};
    int m_expandedWidth{0};
};


#endif //STACKED_WIDGET_H
