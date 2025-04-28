//
// Created by viphg on 2025/3/19.
//

#include "ImageCapturePage.h"
#include <QVBoxLayout>
#include <QFileDialog>
#include <QDesktopServices>
#include <ElaText.h>
#include <QDesktopServices>
#include "FileTools/Microscope_Utils_Config.h"
#include "FileTools/Microscope_Utils_Log.h"

ImageCapturePage::ImageCapturePage(QWidget *parent): QWidget(parent) {
    this->setObjectName("CameraSettingPage");
    this->setMinimumHeight(150);
    auto* layout = new QVBoxLayout(this);
    this->setLayout(layout);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setAlignment(Qt::AlignTop | Qt::AlignCenter);
    layout->setSpacing(10);

    QFont font;
    font.setPixelSize(12);
    font.setFamily("Microsoft YaHei");

    m_screenShotButton = new ElaPushButton(tr("拍照"), this);
    m_screenShotButton->setFont(font);
    m_screenShotButton->setFixedWidth(120);
    m_recordButton = new ElaToggleButton(tr("录像"), this);
    m_recordButton->setFont(font);
    m_recordButton->setFixedWidth(120);
    const auto buttonLayout = new QHBoxLayout;
    buttonLayout->setSpacing(10);
    buttonLayout->addWidget(m_screenShotButton);
    buttonLayout->addWidget(m_recordButton);


    const auto pathLabelLayout = new QHBoxLayout();
    pathLabelLayout->setSpacing(5);

    const auto pathLabel = new ElaText(tr("路径"), this);
    pathLabel->setFont(font);

    m_pathLineEdit = new ElaLineEdit(this);
    m_pathLineEdit->setFont(font);
    m_pathLineEdit->setFixedHeight(30);
    m_pathLineEdit->setReadOnly(true);

    pathLabelLayout->addWidget(pathLabel);
    pathLabelLayout->addWidget(m_pathLineEdit);


    const auto pathChangeLayout = new QHBoxLayout();
    pathChangeLayout->setSpacing(5);

    m_pathSelectButton = new ElaPushButton(tr("设置"), this);
    m_pathSelectButton->setFont(font);
    m_pathSelectButton->setFixedWidth(50);
    m_pathSelectButton->setFixedHeight(30);

    m_pathOpenButton  = new ElaPushButton(tr("打开"), this);
    m_pathOpenButton->setFont(font);
    m_pathOpenButton->setFixedWidth(50);
    m_pathOpenButton->setFixedHeight(30);

    pathChangeLayout->addStretch(1);
    pathChangeLayout->addWidget(m_pathSelectButton);
    pathChangeLayout->addWidget(m_pathOpenButton);


    layout->addItem(buttonLayout);
    layout->addItem(pathLabelLayout);
    layout->addItem(pathChangeLayout);

    initUI();

    connect(m_screenShotButton, &ElaPushButton::clicked, this, &ImageCapturePage::sigScreenShot);
    connect(m_recordButton, &ElaToggleButton::toggled, this, &ImageCapturePage::sigRecord);
    connect(m_pathSelectButton, &ElaPushButton::clicked, this, &ImageCapturePage::onPathSelect);
    connect(m_pathOpenButton, &ElaPushButton::clicked, this, &ImageCapturePage::onPathOpen);
}

void ImageCapturePage::setRecordButtonEnable(const bool flag) const
{
    m_recordButton->setEnabled(flag);
}

void ImageCapturePage::setScreenShotButtonEnable(const bool flag) const
{
    m_screenShotButton->setEnabled(flag);
}

void ImageCapturePage::setRecordButtonChecked(const bool flag) const
{
    if (m_recordButton->getIsToggled() == flag) {
        return;
    }
    m_recordButton->setIsToggled(flag);
}

void ImageCapturePage::initUI() {
    std::string strPath;
    if(configApp->getFilePath(strPath)!=0) {

    }
    m_pathLineEdit->setText(QString::fromStdString(strPath));

}

void ImageCapturePage::onPathSelect() {

    std::string strPath;
    if(configApp->getFilePath(strPath)!=0) {
        strPath = QDir::homePath().toStdString();
    }
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), QString::fromStdString(strPath), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(!dir.isEmpty())
        m_pathLineEdit->setText(dir);
    if(configApp->setFilePath(dir.toStdString())!=0)
    {
        LOG_ERROR("set file path failed");
    }

}

void ImageCapturePage::onPathOpen() {
    QString strPath = m_pathLineEdit->text();
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), strPath, tr("Images (*.png *.jpg *.tiff *.mp4)"));

    QDesktopServices::openUrl ( QUrl::fromLocalFile(fileName) );

    // QProcess* process = new QProcess();
    // QString lastFilePath = fileName;
    //
    // QString sFilePath = lastFilePath.replace("/", "\\");
    // QString program = "cmd.exe";
    // QStringList para;
    // para << "/c";
    // //命令间需要添加空格
    // para << QString("rundll32") + QString(" %Systemroot%\\System32\\shimgvw.dll,VideoView_Fullscreen ")+ sFilePath;
    // process->start(program, para);

}
