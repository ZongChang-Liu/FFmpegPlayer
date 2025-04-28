//
// Created by liu_zongchang on 2025/1/13 0:40.
// Email 1439797751@qq.com
// 
//

#ifndef CAMERA_SWITCHING_PAGE_H
#define CAMERA_SWITCHING_PAGE_H

#include <QWidget>
#include "DirectShow/UVC_Win_DShow.h"

class ElaComboBox;
class CameraSwitchingPage final : public QWidget {
    Q_OBJECT
public:
    explicit CameraSwitchingPage(QWidget *parent = nullptr);
    ~CameraSwitchingPage() override;

    void updateCameraInfo();

    Q_SLOT void onCameraChanged(int index);
    Q_SLOT void onFormatChanged(int index) const;
    Q_SLOT void onResolutionChanged(int index);

    Q_SIGNAL void sigCameraChanged(const CameraDevice& cameraDevice,int width, int height, double fps, QString format);
    Q_SIGNAL void sigScreenShot();
    Q_SIGNAL void sigRecord(bool flag);
private:
    ElaComboBox *m_cameraComboBox;
    ElaComboBox *m_formatComboBox;
    ElaComboBox *m_resolutionComboBox;

    std::vector<CameraDevice> m_cameraList;
    std::vector<DeviceResolution> m_resolutionList;
};



#endif //CAMERA_SWITCHING_PAGE_H
