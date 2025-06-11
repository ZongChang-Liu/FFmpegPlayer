//
// Created by liu_zongchang on 2025/3/25 18:52.
// Email 1439797751@qq.com
//

#include "ToolScrollWidget.h"

#include <QVBoxLayout>
#include <QTimer>
#include "ElaDrawerArea.h"
#include "ElaScrollArea.h"
#include "ElaScrollBar.h"
#include "ElaText.h"

ToolScrollWidget::ToolScrollWidget(QWidget* parent) : QWidget(parent)
{
    this->setFixedWidth(250);
    this->setLayout(new QVBoxLayout(this));
    this->layout()->setContentsMargins(0, 0, 0, 0);

    m_scrollWidget = new QWidget(this);
    m_scrollWidget->setObjectName("ToolScrollWidget");
    m_scrollWidget->setStyleSheet("#ToolScrollWidget{background-color:transparent;}");
    m_scrollWidget->setLayout(new QVBoxLayout(m_scrollWidget));
    m_scrollWidget->layout()->setAlignment(Qt::AlignTop | Qt::AlignVCenter);
    m_scrollWidget->layout()->setContentsMargins(10, 10, 10, 10);
    m_scrollWidget->layout()->setSpacing(10);

    m_scrollArea = new ElaScrollArea(this);
    m_scrollArea->setWidget(m_scrollWidget);
    m_scrollArea->setWidgetResizable(true);
    auto* floatVScrollBar = new ElaScrollBar(m_scrollArea->verticalScrollBar(), m_scrollArea);
    floatVScrollBar->setIsAnimation(true);

    this->layout()->addWidget(m_scrollArea);
}

ToolScrollWidget::~ToolScrollWidget() = default;

void ToolScrollWidget::addPageNode(const QString& title, QWidget* widget)
{
    auto* drawer = new ElaDrawerArea(this);
    connect(drawer, &ElaDrawerArea::expandStateChanged, this,&ToolScrollWidget::onUpdateFloatVScrollBar);

    const auto drawerText = new ElaText(title, this);
    drawerText->setMargin(10);
    drawerText->setTextPixelSize(15);

    drawer->setDrawerHeader(drawerText);
    drawer->setHeaderHeight(35);
    // drawer->setBorderRadius(1);
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


    onUpdateFloatVScrollBar();
}

void ToolScrollWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    onUpdateFloatVScrollBar();
}

void ToolScrollWidget::onUpdateFloatVScrollBar() const
{
    QTimer::singleShot(285, this, [this]() {
        if (m_scrollArea->height() < m_scrollWidget->layout()->minimumSize().height())
        {
            m_scrollWidget->layout()->setContentsMargins(5, 10, 15, 10);
        } else
        {
            m_scrollWidget->layout()->setContentsMargins(10, 10, 10, 10);
        }
    });
}
