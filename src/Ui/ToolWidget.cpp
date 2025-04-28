//
// Created by liu_zongchang on 2025/3/25 18:52.
// Email 1439797751@qq.com
//

#include "ToolWidget.h"

#include <QVBoxLayout>
#include <ElaDrawerArea.h>
#include <ElaScrollArea.h>
#include <ElaScrollBar.h>
#include <ElaText.h>

ToolWidget::ToolWidget(QWidget* parent) : QWidget(parent)
{
    this->setMinimumWidth(300);
    this->setLayout(new QVBoxLayout(this));
    this->layout()->setContentsMargins(0, 0, 0, 0);

    m_scrollWidget = new QWidget(this);
    m_scrollWidget->setObjectName("ToolScrollWidget");
    m_scrollWidget->setStyleSheet("#ToolScrollWidget{background-color:transparent;}");
    m_scrollWidget->setLayout(new QVBoxLayout(m_scrollWidget));
    m_scrollWidget->layout()->setContentsMargins(5, 10, 15, 10);
    m_scrollWidget->layout()->setSpacing(10);

    const auto scrollArea = new ElaScrollArea(this);
    scrollArea->setWidget(m_scrollWidget);
    scrollArea->setWidgetResizable(true);
    auto* floatVScrollBar = new ElaScrollBar(scrollArea->verticalScrollBar(), scrollArea);
    floatVScrollBar->setIsAnimation(true);

    this->layout()->addWidget(scrollArea);
}

ToolWidget::~ToolWidget() = default;

void ToolWidget::addPageNode(const QString& title, QWidget* widget)
{
    auto* drawer = new ElaDrawerArea(this);

    const auto drawerText = new ElaText(title, this);
    drawerText->setMargin(10);
    drawerText->setTextPixelSize(15);

    drawer->setDrawerHeader(drawerText);
    drawer->setHeaderHeight(50);
    drawer->addDrawer(widget);
    drawer->expand();

    if (auto* layout = qobject_cast<QVBoxLayout*>(m_scrollWidget->layout()))
    {
        if (layout->count() > 1)
        {
            layout->removeItem(layout->itemAt(layout->count() - 1));
        }

        layout->addWidget(drawer);
        layout->addStretch();
    }
}
