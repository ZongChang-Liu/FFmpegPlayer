//
// Created by liu_zongchang on 2025/1/13 18:09.
// Email 1439797751@qq.com
// 
//

#include <QApplication>
#include <QLabel>
#include <QVBoxLayout>

#include <ElaPushButton.h>
#include <ElaText.h>
#include <ElaCheckBox.h>
#include <ElaScrollPageArea.h>
#include <ElaSlider.h>
#include <ElaScrollBar.h>
#include <ElaScrollArea.h>
#include "CameraSettingPage.h"

CameraSettingPage::CameraSettingPage(QWidget* parent) : QWidget(parent)
{
    this->setObjectName("CameraSettingPage");
    this->setLayout(new QVBoxLayout(this));
    this->layout()->setContentsMargins(0, 0, 0, 0);
    this->layout()->setSpacing(0);
    this->layout()->setAlignment(Qt::AlignTop);
    this->setMinimumHeight(350);

    QFont font;
    font.setPixelSize(12);
    font.setFamily("Microsoft YaHei");
    m_cameraInfoLabel = new ElaText(this);
    m_cameraInfoLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_cameraInfoLabel->setFixedHeight(40);
    m_cameraInfoLabel->setFont(font);
    m_cameraInfoLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    m_resetButton = new ElaPushButton(tr("复位"), this);
    m_resetButton->setFont(font);
    m_resetButton->setFixedWidth(70);
    m_resetButton->setFixedHeight(30);

    auto* cameraLayout = new QHBoxLayout();
    cameraLayout->setContentsMargins(10, 10, 10, 10);
    cameraLayout->setSpacing(10);
    cameraLayout->addWidget(m_cameraInfoLabel);
    cameraLayout->addWidget(m_resetButton);
    m_cameraInfoLabel->setVisible(false);
    m_resetButton->setVisible(false);

    m_scrollWidget = new QWidget(this);
    m_scrollWidget->setObjectName("CameraSettingScrollWidget");
    m_scrollWidget->setStyleSheet("#CameraSettingScrollWidget{background-color:transparent;}");
    m_scrollWidget->setLayout(new QVBoxLayout(m_scrollWidget));
    m_scrollWidget->layout()->setContentsMargins(15, 10, 15, 20);
    m_scrollWidget->layout()->setSpacing(10);

    const auto scrollArea = new ElaScrollArea(this);
    scrollArea->setWidget(m_scrollWidget);
    scrollArea->setWidgetResizable(true);
    auto* floatVScrollBar = new ElaScrollBar(scrollArea->verticalScrollBar(), scrollArea);
    floatVScrollBar->setIsAnimation(true);

    this->layout()->addItem(cameraLayout);
    this->layout()->addWidget(scrollArea);
}

void CameraSettingPage::clearParameter()
{
    if (auto* layout = m_scrollWidget->layout()) {
        QLayoutItem* item;
        while ((item = layout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
    }
}

void CameraSettingPage::updateParameter(const CameraDevice& cameraInfo, const int width, const int height, const double fps)
{
    m_cameraInfoLabel->setText(tr("%1 %2x%3").arg(cameraInfo.FriendlyName).arg(width).arg(height));
    if (m_cameraInfo.MonikerName == cameraInfo.MonikerName) {
        return;
    }
    m_cameraInfoLabel->setVisible(true);
    m_cameraInfo = cameraInfo;
    UVC_Win_DShow::getParameters(m_cameraInfo,m_parameters);
    clearParameter();
    if (m_parameters.empty()) {
        m_resetButton->setVisible(false);
        return;
    }
    m_resetButton->setVisible(true);
    for (const auto& parameter : m_parameters) {
        addParameterItem(parameter);
    }
}

void CameraSettingPage::addParameterItem(const DeviceParameter& parameter)
{
    auto* scrollPage = new ElaScrollPageArea(this);
    scrollPage->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    scrollPage->setLayout(new QVBoxLayout);

    auto* showLayout = new QHBoxLayout();
    showLayout->setContentsMargins(0, 0, 0, 0);
    showLayout->setSpacing(5);

    auto* controlLayout = new QHBoxLayout();
    controlLayout->setContentsMargins(0, 0, 0, 0);
    controlLayout->setSpacing(10);

    QFont font;
    font.setPixelSize(14);
    font.setFamily("Microsoft YaHei");

    auto* label = new ElaText(scrollPage);
    const auto type = ParameterTypes.at(parameter.dataType);
    label->setText(QApplication::translate("CameraSettingPage", type,nullptr));
    label->setFont(font);
    label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    label->setFixedHeight(30);

    auto* value = new ElaText(scrollPage);
    value->setText(QString::number(parameter.value));
    value->setFont(font);
    value->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    value->setFixedHeight(30);

    auto* defButton = new ElaPushButton(tr("复位"), scrollPage);
    defButton->setFont(font);
    defButton->setFixedWidth(70);
    defButton->setFixedHeight(30);


    showLayout->addWidget(label);
    showLayout->addWidget(value);
    showLayout->addStretch();
    showLayout->addWidget(defButton);

    auto* isAutoCheckBox = new ElaCheckBox(tr("自动"), scrollPage);
    isAutoCheckBox->setFont(font);
    isAutoCheckBox->setChecked(parameter.isAuto);
    isAutoCheckBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);if (parameter.autoIsEnable) {
        isAutoCheckBox->setEnabled(true);
    } else {
        isAutoCheckBox->setEnabled(false);
    }

    auto* slider = new ElaSlider(scrollPage);
    slider->setRange(parameter.min, parameter.max);
    slider->setValue(parameter.value);
    slider->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    slider->setEnabled(!parameter.isAuto);

    controlLayout->addWidget(slider);
    controlLayout->addWidget(isAutoCheckBox);

    scrollPage->layout()->addItem(showLayout);
    scrollPage->layout()->addItem(controlLayout);

    m_scrollWidget->layout()->addWidget(scrollPage);

    connect(m_resetButton, &ElaPushButton::clicked, [this, defButton]() {
        defButton->click();
    });

    connect(defButton, &ElaPushButton::clicked, [this, parameter, slider]() {
        if (slider->value() != parameter.def) {
            slider->setValue(parameter.def);
            UVC_Win_DShow::setParameterDefault(m_cameraInfo, parameter.dataType);
        }
    });

    connect(slider, &ElaSlider::valueChanged, [this, parameter, value](const int val) {
       value->setText(QString::number(val));
       UVC_Win_DShow::setParameter(m_cameraInfo, parameter.dataType, val, false);
   });
    connect(isAutoCheckBox, &ElaCheckBox::toggled, [this, parameter, slider](const bool flag) {
        slider->setEnabled(!flag);
        slider->setValue(parameter.def);
        UVC_Win_DShow::setParameter(m_cameraInfo, parameter.dataType, parameter.def, flag);
    });
}
