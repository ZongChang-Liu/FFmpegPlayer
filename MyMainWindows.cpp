//
// Created by liu_zongchang on 2025/1/7 23:04.
// Email 1439797751@qq.com
// 
//

#include "MyMainWindows.h"


#include <QKeyEvent>
#include <QDateTime>
#include <QVBoxLayout>
#include <QMetaEnum>
#include <QProcess>
#include <QCoreApplication>
#include <QTranslator>
#include <QLocale>
#include <QActionGroup>

#include <ElaDockWidget.h>
#include <ElaMessageBar.h>
#include <ElaMenuBar.h>
#include <ElaStatusBar.h>

#include "FileTools/Microscope_Utils_Config.h"
#include "FileTools/Microscope_Utils_Log.h"
#include "OperateWidget/CameraSwitchingPage.h"
#include "OperateWidget/CameraSettingPage.h"
#include "OperateWidget/ImageCapturePage.h"
#include "FFmpeg/FFmpegPlayer.h"
#include "FFmpeg/FFmpegRecorder.h"
#include "Ui/ToolScrollWidget.h"
#include "Ui/VideoWidget.h"

MyMainWindows::MyMainWindows(QWidget* parent) : ElaWindow(parent)
{
    this->setWindowIcon(QIcon(":/resources/logo.ico"));
    this->setMinimumSize(800, 600);
    this->setWindowTitle(tr("FFmpegPlayer"));
    this->setIsStayTop(false);
    this->setIsNavigationBarEnable(false);
    this->setIsCentralStackedWidgetTransparent(true);
    this->setWindowButtonFlags(
        ElaAppBarType::MinimizeButtonHint | ElaAppBarType::MaximizeButtonHint | ElaAppBarType::CloseButtonHint |
        ElaAppBarType::ThemeChangeButtonHint);
    this->moveToCenter();

    initSystem();
    initEdgeLayout();
    initContentLayout();

    m_ffmpegPlayer = new FFmpegPlayer(this);
    m_ffmpegRecorder = new FFmpegRecorder(this);
    connect(m_ffmpegPlayer, &FFmpegPlayer::sigFrameReaded, m_videoWidget, &VideoWidget::onUpdateFrame);
    connect(m_ffmpegPlayer, &FFmpegPlayer::sigPlayStatusChange, this, &MyMainWindows::onPlayStatusChange);
    connect(m_ffmpegPlayer, &FFmpegPlayer::sigFrameReaded, m_ffmpegRecorder, &FFmpegRecorder::onFrameReceived);

    connect(m_ffmpegRecorder, &FFmpegRecorder::sigRecordStatusChange, this, &MyMainWindows::onRecordStatusChange);

    connect(m_cameraSwitchingPage, &CameraSwitchingPage::sigCameraChanged, this, &MyMainWindows::onCameraChanged);
    connect(m_imageCapturePage, &ImageCapturePage::sigRecord,this, &MyMainWindows::onCameraRecord);
    connect(m_imageCapturePage, &ImageCapturePage::sigScreenShot, this, &MyMainWindows::onCameraScreenShot);

    m_cameraSwitchingPage->updateCameraInfo();
}

MyMainWindows::~MyMainWindows()
{
    if (m_ffmpegPlayer)
    {
        m_ffmpegPlayer->close();
        delete m_ffmpegPlayer;
    }

    if (m_ffmpegRecorder)
    {
        m_ffmpegRecorder->stopRecord();
        delete m_ffmpegRecorder;
    }
}

void MyMainWindows::initSystem() const
{
#ifdef Q_OS_WIN
    DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
    ZeroMemory(&NotificationFilter, sizeof(NotificationFilter));
    NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    NotificationFilter.dbcc_classguid = CAMERA;
    //注册相机设备
    HDEVNOTIFY hDevNotify = RegisterDeviceNotification(reinterpret_cast<HANDLE>(this->winId()), &NotificationFilter,
                                                       DEVICE_NOTIFY_WINDOW_HANDLE);
    if (hDevNotify == nullptr)
    {
        LOG_ERROR("CAMERA Register Failed");
    }
    else
    {
        LOG_INFO("CAMERA Register Success");
    }

    ZeroMemory(&NotificationFilter, sizeof(NotificationFilter));
    NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    NotificationFilter.dbcc_classguid = COMPORT;
    //注册串口设备
    hDevNotify = RegisterDeviceNotification(reinterpret_cast<HANDLE>(this->winId()), &NotificationFilter,
                                            DEVICE_NOTIFY_WINDOW_HANDLE);
    if (hDevNotify == nullptr)
    {
        LOG_ERROR("COM Register Failed");
    }
    else
    {
        LOG_INFO("COM Register Success");
    }

    ZeroMemory(&NotificationFilter, sizeof(NotificationFilter));
    NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    NotificationFilter.dbcc_classguid = USB;
    //注册串口设备
    hDevNotify = RegisterDeviceNotification(reinterpret_cast<HANDLE>(this->winId()), &NotificationFilter,
                                            DEVICE_NOTIFY_WINDOW_HANDLE);
    if (hDevNotify == nullptr)
    {
        LOG_ERROR("USB Register Failed");
    }
    else
    {
        LOG_INFO("USB Register Success");
    }
#endif
}

void MyMainWindows::initEdgeLayout()
{
    auto* menuBar = new ElaMenuBar(this);
    menuBar->setFixedHeight(30);
    auto* customWidget = new QWidget(this);
    auto* customLayout = new QVBoxLayout(customWidget);
    customLayout->setContentsMargins(0, 0, 0, 0);
    customLayout->addWidget(menuBar);
    customLayout->addStretch();
    this->setCustomWidget(ElaAppBarType::LeftArea, customWidget);

    auto* group = new QActionGroup(this);
    auto* menu = new ElaMenu(tr("语言"), this);
    auto* action_zh_CN = new QAction("中文", this);
    action_zh_CN->setObjectName("zh_CN");
    action_zh_CN->setCheckable(true);
    auto* action_en = new QAction("English", this);
    action_en->setObjectName("en");
    action_en->setCheckable(true);
    group->addAction(action_zh_CN);
    group->addAction(action_en);

    std::string translator_config;
    if (configApp->getTranslator(translator_config) == 0)
    {
        LOG_INFO("current date: {}",translator_config);
        if (translator_config == "en")
        {
            action_en->setChecked(true);
        }
        else
        {
            action_zh_CN->setChecked(true);
        }
    } else
    {
        if (QLocale::system().name() == "zh_CN")
        {
            action_zh_CN->setChecked(true);
        }
        else
        {
            action_en->setChecked(true);
        }
    }


    connect(action_zh_CN, &QAction::triggered, this,&MyMainWindows::onTranslatorChanged);
    connect(action_en, &QAction::triggered, this, &MyMainWindows::onTranslatorChanged);
    menu->addAction(action_zh_CN);
    menu->addAction(action_en);
    menuBar->addMenu(menu);

    auto* statusBar = new ElaStatusBar(this);
    m_statusText = new ElaText(tr("系统初始化成功! "), this);
    m_statusText->setTextPixelSize(12);
    statusBar->addWidget(m_statusText);
    this->setStatusBar(statusBar);
}

void MyMainWindows::initContentLayout()
{
    m_contentWidget = new QWidget(this);
    m_contentWidget->setObjectName("MainContent");
    auto* layout = new QHBoxLayout(m_contentWidget);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(0);
    addPageNode("Main", m_contentWidget, ElaIconType::House);

    m_videoWidget = new VideoWidget(this);
    m_toolWidget = new ToolScrollWidget(this);
    m_toolWidget->setFixedWidth(300);

    layout->addWidget(m_videoWidget,4);
    layout->addWidget(m_toolWidget,1);

    m_cameraSwitchingPage = new CameraSwitchingPage(this);
    m_imageCapturePage = new ImageCapturePage(this);
    m_cameraSettingPage = new CameraSettingPage(this);


    m_toolWidget->addPageNode(tr("相机设置"), m_cameraSwitchingPage);
    m_toolWidget->addPageNode(tr("图像捕捉设置"), m_imageCapturePage);
    m_toolWidget->addPageNode(tr("相机参数设置"), m_cameraSettingPage);
}

void MyMainWindows::createDockWidget(const QString& title, QWidget* widget, const Qt::DockWidgetArea area)
{
    auto* dockWidget = new ElaDockWidget(title, this);
    dockWidget->setObjectName(title);
    widget->setParent(dockWidget);
    //dockWidget->setDockWidgetTitleIconVisible(false);
    dockWidget->setWidget(widget);
    this->addDockWidget(area, dockWidget);
    this->resizeDocks({dockWidget}, {250}, Qt::Horizontal);

    auto* action = dockWidget->toggleViewAction();
    action->setText(title);
    action->setCheckable(true);
    action->setChecked(true);
    m_docketMenu->addAction(action);
}

void MyMainWindows::onTranslatorChanged(bool checked) const
{
    const auto* action = qobject_cast<QAction*>(sender());
    if (!action)
    {
        return;
    }

    std::string translator_config;
    if (configApp->getTranslator(translator_config) != 0)
    {
        if (QLocale::system().name() == "zh_CN")
        {
            configApp->setTranslator("zh_CN");
            translator_config = "zh_CN";
        }
        else
        {
            configApp->setTranslator("en");
            translator_config = "en";
        }
    }

    if (translator_config == action->objectName().toStdString())
    {
        LOG_INFO("current translator_config is same");
        return;
    }


    if (configApp->setTranslator(action->objectName().toStdString()) == 0)
    {
        LOG_INFO("restart application");
        qApp->quit();
        QProcess::startDetached(QCoreApplication::applicationFilePath(), QStringList());
    }
}

void MyMainWindows::onCameraChanged(const CameraDevice& cameraDevice, const int width, const int height, const double fps,
                                    const QString& format) const
{
    QString ffmpegPath = cameraDevice.MonikerName;
    ffmpegPath.replace(":", "_");
    LOG_INFO("path: {} width: {} height: {} fps: {} format: {}", ffmpegPath.toStdString().c_str(), width, height, fps,
             format.toStdString().c_str());
    m_ffmpegPlayer->playDevice(ffmpegPath, width, height, fps, format.toLower());
    m_cameraSettingPage->updateParameter(cameraDevice, width, height, fps);
}

void MyMainWindows::onCameraRecord(const bool record) const
{
    if (record)
    {
        std::string strPath;
        if(configApp->getFilePath(strPath)!=0)
        {
            LOG_ERROR("get file path failed");
            return;
        }

        const QString current_date = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss");
        const QString path =   QString::fromStdString(strPath) +"/" + current_date + ".mp4";

        const int width = m_ffmpegPlayer->getFrameWidth();
        const int height = m_ffmpegPlayer->getFrameHeight();
        const int fps = m_ffmpegPlayer->getFps();

        if (width <= 0 || height <= 0 || fps <= 0)
        {
            LOG_ERROR("record failed, please check camera device\n");
            return;
        }
        LOG_INFO("record path: {} width: {} height: {} fps: {}", path.toStdString(), width, height, fps);
        m_ffmpegRecorder->startRecord(path.toStdString(), width, height, fps);
    }
    else
    {
        m_ffmpegRecorder->stopRecord();
    }
}

void MyMainWindows::onCameraScreenShot() const
{
    QImage image;
    if(m_videoWidget->getImage(image)==-1)
    {
        LOG_ERROR("screen shot failed");
        return;
    }

    std::string strPath;
    if(configApp->getFilePath(strPath)!=0)
    {
        LOG_ERROR("get file path failed");
        return;
    }


    const QString current_date = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss");
    const QString path =  QString::fromStdString(strPath) + "/" + current_date + ".tiff";

    if (!image.save(path))
    {
        LOG_ERROR("save screen shot failed");
        ElaMessageBar::error(ElaMessageBarType::TopLeft, tr("截图失败"), tr("请检查路径或者权限"), 1000);
        return;
    }
    LOG_INFO("screen shot path: {}", path.toStdString());
    ElaMessageBar::success(ElaMessageBarType::TopLeft, tr("截图成功"), tr("保存路径:") + path, 1000);
}

void MyMainWindows::onPlayStatusChange(const int status) const
{
    switch (status)
    {
    case FFmpegPlayer::Status::STATUS_ERROR:
        {
            ElaMessageBar::error(ElaMessageBarType::TopLeft, tr("播放错误"), tr("请检查相机设备"), 1000);
            m_cameraSwitchingPage->setEnabled(true);
            break;
        }
    case FFmpegPlayer::Status::STATUS_INITIALIZING:
        {
            ElaMessageBar::information(ElaMessageBarType::TopLeft, tr("初始化中"), tr("请稍等"), 1000);
            m_cameraSwitchingPage->setEnabled(false);
            break;
        }
    case FFmpegPlayer::Status::STATUS_PLAYING:
        {
            ElaMessageBar::success(ElaMessageBarType::TopLeft, tr("播放成功"), tr("请查看视频"), 1000);
            m_cameraSwitchingPage->setEnabled(true);
            m_imageCapturePage->setRecordButtonEnable(true);
            m_imageCapturePage->setScreenShotButtonEnable(true);
            break;
        }
    case FFmpegPlayer::Status::STATUS_STOPPED:
        {
            ElaMessageBar::information(ElaMessageBarType::TopLeft, tr("播放停止"), tr("设备已关闭"), 1000);
            m_ffmpegRecorder->stopRecord();
            m_cameraSwitchingPage->setEnabled(true);
            m_imageCapturePage->setRecordButtonEnable(false);
            m_imageCapturePage->setScreenShotButtonEnable(false);
            break;
        }
    default: break;
    }
    LOG_INFO("play status: {}", QMetaEnum::fromType<FFmpegPlayer::Status>().valueToKey(status));
}

void MyMainWindows::onRecordStatusChange(const int status) const
{
    switch (status)
    {
        case FFmpegRecorder::Status::STATUS_ERROR:
        {
            ElaMessageBar::error(ElaMessageBarType::TopLeft, tr("录制错误"), tr("请检查路径或者权限"), 1000);
            m_imageCapturePage->setRecordButtonChecked(false);
            break;
        }
        case FFmpegRecorder::Status::STATUS_INITIALIZING:
        {
            ElaMessageBar::information(ElaMessageBarType::TopLeft, tr("初始化中"), tr("请稍等..."), 1000);
            m_imageCapturePage->setRecordButtonEnable(false);
            break;
        }
        case FFmpegRecorder::Status::STATUS_RECORDING:
        {
            ElaMessageBar::success(ElaMessageBarType::TopLeft, tr("录制中"), tr("请稍等..."), 1000);
            m_imageCapturePage->setRecordButtonEnable(true);
            m_videoWidget->setRecordFlag(true);
            break;
        }
        case FFmpegRecorder::Status::STATUS_STOPPED:
        {
            ElaMessageBar::information(ElaMessageBarType::TopLeft, tr("录制停止"), tr("录制已结束"), 1000);
            m_imageCapturePage->setRecordButtonChecked(false);
            m_imageCapturePage->setRecordButtonEnable(true);
            m_videoWidget->setRecordFlag(false);
            break;
        }
        case FFmpegRecorder::Status::STATUS_OK:
        {
            ElaMessageBar::success(ElaMessageBarType::TopLeft, tr("录制成功"), tr("请查看视频"), 1000);
            m_imageCapturePage->setRecordButtonEnable(true);
            break;
        }
    default: break;
    }
    LOG_INFO("record status: {}", QMetaEnum::fromType<FFmpegRecorder::Status>().valueToKey(status));
}

void MyMainWindows::keyPressEvent(QKeyEvent* event)
{
    QMainWindow::keyPressEvent(event);
    if (event->key() == Qt::Key_Escape)
    {
        close();
    }
}

#ifdef Q_OS_WIN
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
bool MyMainWindows::nativeEvent(const QByteArray &eventType, void *message, qintptr *result) {
#else
bool MyMainWindows::nativeEvent(const QByteArray& eventType, void* message, long* result)
{
#endif

    Q_UNUSED(eventType)
    MSG* msg = static_cast<MSG*>(message);
    switch (msg->message)
    {
    case WM_DEVICECHANGE:
        {
            if (msg->wParam == DBT_DEVICEARRIVAL || msg->wParam == DBT_DEVICEREMOVECOMPLETE)
            {
                if (const auto dev_broadcast_hdr = reinterpret_cast<PDEV_BROADCAST_HDR>(msg->lParam); dev_broadcast_hdr
                    ->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE)
                {
                    if (const auto dev_broadcast_device_interface = reinterpret_cast<PDEV_BROADCAST_DEVICEINTERFACE>(
                        dev_broadcast_hdr); dev_broadcast_device_interface->dbcc_classguid == CAMERA)
                    {
                        m_cameraSwitchingPage->updateCameraInfo();
                    }
                }
            }
            break;
        }
    }

    return ElaWindow::nativeEvent(eventType, message, result);
}

#endif
