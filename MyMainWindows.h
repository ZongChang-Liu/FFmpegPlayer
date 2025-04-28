//
// Created by liu_zongchang on 2025/1/7 23:04.
// Email 1439797751@qq.com
// 
//

#ifndef FFMPEG_PLAYER_MY_MAIN_WINDOWS_H
#define FFMPEG_PLAYER_MY_MAIN_WINDOWS_H

#include "ElaWindow.h"
#include "ElaMenu.h"
#include "ElaText.h"
#include "DirectShow/UVC_Win_DShow.h"
#pragma execution_character_set(push, "utf-8")


#ifdef Q_OS_WIN
#include <windows.h>
#include <dwmapi.h>
#include <Dbt.h>
#include <atlbase.h>
#include <atlwin.h>
#endif


class VideoWidget;
class FFmpegPlayer;
class FFmpegRecorder;
class CameraSwitchingPage;
class CameraSettingPage;
class ImageCapturePage;
class ToolWidget;

class MyMainWindows final : public ElaWindow {
Q_OBJECT
public:
    explicit MyMainWindows(QWidget *parent = nullptr);
    ~MyMainWindows() override;

    void initSystem() const;
    void initEdgeLayout();
    void initContentLayout();

    void createDockWidget(const QString &title, QWidget *widget, Qt::DockWidgetArea area);

    Q_SLOT void onCameraChanged(const CameraDevice& cameraDevice, int width, int height, double fps, const QString& format) const;
    Q_SLOT void onCameraRecord(bool record) const;
    Q_SLOT void onCameraScreenShot() const;

    Q_SLOT void onPlayStatusChange(int status) const;
    Q_SLOT void onRecordStatusChange(int status) const;

    Q_SLOT void onTranslatorChanged(bool checked) const;

    Q_SLOT void keyPressEvent(QKeyEvent* event) override;

#ifdef Q_OS_WIN
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;
#else
    bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;
#endif
#endif

private:
    ElaMenu *m_docketMenu = nullptr;
    ElaText *m_statusText{nullptr};
    QWidget *m_contentWidget{nullptr};
    FFmpegPlayer *m_ffmpegPlayer{nullptr};
    FFmpegRecorder *m_ffmpegRecorder{nullptr};
    VideoWidget *m_videoWidget{nullptr};
    ToolWidget *m_toolWidget{nullptr};

    ImageCapturePage *m_imageCapturePage{nullptr};
    CameraSwitchingPage *m_cameraSwitchingPage{nullptr};
    CameraSettingPage *m_cameraSettingPage{nullptr};
#ifdef Q_OS_WIN
    GUID CAMERA = {0xE5323777, 0xF976, 0x4f5b, { 0x9B, 0x55, 0xB9, 0x46, 0x99, 0xC4, 0x6E, 0x44}};
    GUID COMPORT = {0x86E0D1E0, 0x8089, 0x11D0, { 0x9C, 0xE4, 0x08, 0x00, 0x3E, 0x30, 0x1F, 0x73}};
    GUID USB = {0xA5DCBF10, 0x6530, 0x11D2, { 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED}};
#endif
};



#endif //FFMPEG_PLAYER_MY_MAIN_WINDOWS_H
