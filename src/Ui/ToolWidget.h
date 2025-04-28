//
// Created by liu_zongchang on 2025/3/25 18:52.
// Email 1439797751@qq.com
//

#ifndef TOOL_WIDGET_H
#define TOOL_WIDGET_H

#include <QWidget>
class ElaScrollPageArea;

class ToolWidget final : public QWidget {
    Q_OBJECT
public:
    explicit ToolWidget(QWidget* parent = nullptr);
    ~ToolWidget() override;

    void addPageNode(const QString& title, QWidget* widget);

private:
    QWidget* m_scrollWidget{nullptr};
};



#endif //TOOL_WIDGET_H
