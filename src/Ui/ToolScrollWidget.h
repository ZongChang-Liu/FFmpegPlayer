//
// Created by liu_zongchang on 2025/3/25 18:52.
// Email 1439797751@qq.com
//

#ifndef TOOL_WIDGET_H
#define TOOL_WIDGET_H

#include <QWidget>
class ElaScrollArea;

class ToolScrollWidget final : public QWidget {
    Q_OBJECT
public:
    explicit ToolScrollWidget(QWidget* parent = nullptr);
    ~ToolScrollWidget() override;

    void addPageNode(const QString& title, QWidget* widget);
    void resizeEvent(QResizeEvent* event) override;
protected:
   Q_SLOT void onUpdateFloatVScrollBar() const;
private:
    QWidget* m_scrollWidget{nullptr};
    ElaScrollArea* m_scrollArea{nullptr};
};



#endif //TOOL_WIDGET_H
