//
// Created by liu_zongchang on 2025/3/19 10:40.
// Email 1439797751@qq.com
//

#ifndef IMAGE_CAPTURE_PAGE_H
#define IMAGE_CAPTURE_PAGE_H


#include <ElaPushButton.h>
#include <ElaToggleButton.h>
#include <ElaLineEdit.h>
#include "DirectShow/UVC_Win_DShow.h"

class ImageCapturePage final : public QWidget{
    Q_OBJECT
public:
    explicit ImageCapturePage(QWidget *parent = nullptr);
    ~ImageCapturePage() override = default;

    void setRecordButtonEnable(bool flag) const;
    void setScreenShotButtonEnable(bool flag) const;
    void setRecordButtonChecked(bool flag) const;

    void initUI();

    Q_SIGNAL void sigScreenShot();
    Q_SIGNAL void sigRecord(bool flag);

    Q_SLOT void onPathSelect();
    Q_SLOT void onPathOpen();

private:
    ElaPushButton *m_screenShotButton;
    ElaToggleButton *m_recordButton;
    ElaPushButton *m_pathSelectButton;
    ElaPushButton *m_pathOpenButton;

    ElaLineEdit *m_pathLineEdit;
};



#endif //IMAGE_CAPTURE_PAGE_H
