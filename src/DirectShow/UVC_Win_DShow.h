//
// Created by liu_zongchang on 2025/1/8 4:01.
// Email 1439797751@qq.com
// 
//

#ifndef UVC_WIN_DSHOW_H
#define UVC_WIN_DSHOW_H

#include <windows.h>
#include <dshow.h>
#include <unordered_map>
#include <iostream>
#include <mutex>

#pragma execution_character_set(push, "utf-8")
#pragma comment(lib, "Strmiids.lib")

struct GUIDHash {
    std::size_t operator()(const GUID& guid) const {
        const auto* p = reinterpret_cast<const uint64_t*>(&guid);
        return std::hash<uint64_t>()(p[0]) ^ std::hash<uint64_t>()(p[1]);
    }
};
static const std::unordered_map<GUID, const char*, GUIDHash> MediaSubTypes = {
    {MEDIASUBTYPE_CLPL, "CLPL"},
    {MEDIASUBTYPE_YUYV, "YUYV"},
    {MEDIASUBTYPE_IYUV, "IYUV"},
    {MEDIASUBTYPE_YVU9, "YVU9"},
    {MEDIASUBTYPE_Y411, "Y411"},
    {MEDIASUBTYPE_Y41P, "Y41P"},
    {MEDIASUBTYPE_YUY2, "YUY2"},
    {MEDIASUBTYPE_UYVY, "YVYU"},
    {MEDIASUBTYPE_Y211, "Y211"},
    {MEDIASUBTYPE_CLJR, "CLJR"},
    {MEDIASUBTYPE_IF09, "IF09"},
    {MEDIASUBTYPE_CPLA, "CPLA"},
    {MEDIASUBTYPE_MJPG, "MJPG"},
    {MEDIASUBTYPE_TVMJ, "TVMJ"},
    {MEDIASUBTYPE_WAKE, "WAKE"},
    {MEDIASUBTYPE_CFCC, "CFCC"},
    {MEDIASUBTYPE_IJPG, "IJPG"},
    {MEDIASUBTYPE_Plum, "Plum"},
    {MEDIASUBTYPE_DVCS, "DVCS"},
    {MEDIASUBTYPE_H264, "H264"},
    {MEDIASUBTYPE_DVSD, "DVSD"},
    {MEDIASUBTYPE_MDVF, "MDVF"},
    {MEDIASUBTYPE_RGB1, "RGB1"},
    {MEDIASUBTYPE_RGB4, "RGB4"},
    {MEDIASUBTYPE_RGB8, "RGB8"},
    {MEDIASUBTYPE_RGB565, "RGB565"},
    {MEDIASUBTYPE_RGB555, "RGB555"},
    {MEDIASUBTYPE_RGB24, "RGB24"},
    {MEDIASUBTYPE_RGB32, "RGB32"},
    {MEDIASUBTYPE_NV12, "NV12"},
    {{0x35363248, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}, "H265"},
    {{0x30323449, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}, "I420"},
};


enum ParameterType {
    //VideoProcAmpProperty
    Brightness = 0,
    Contrast,
    Hue,
    Saturation,
    Sharpness,
    Gamma,
    ColorEnable,
    WhiteBalance,
    BacklightCompensation,
    Gain,
    //CameraControlProperty
    Pan,
    Tilt,
    Roll,
    Zoom,
    Exposure,
    Iris,
    Focus,
};

static const std::unordered_map<ParameterType,const char* > ParameterTypes = {
    //VideoProcAmpProperty
    {Brightness,            "亮度"},
    {Contrast,              "对比度"},
    {Hue,                   "色调"},
    {Saturation,            "饱和度"},
    {Sharpness,             "锐度"},
    {Gamma,                 "伽马"},
    {ColorEnable,           "色彩启用"},
    {WhiteBalance,          "白平衡"},
    {BacklightCompensation, "背光补偿"},
    {Gain,                  "增益"},
    //CameraControlProperty
    {Pan,                   "平移"},
    {Tilt,                  "倾斜"},
    {Roll,                  "滚动"},
    {Zoom,                  "缩放"},
    {Exposure,              "曝光"},
    {Iris,                  "光圈"},
    {Focus,                 "焦点"},
};

struct DeviceResolution {
    GUID mediaSubType{};
    int width{0};
    int height{0};
    double fps{0};

    bool operator==(const DeviceResolution &resolution) const {
        return width == resolution.width && height == resolution.height && fps == resolution.fps &&
            mediaSubType == resolution.mediaSubType;
    }
};

struct DeviceParameter {
    ParameterType dataType{};
    long min{0};
    long max{0};
    long def{0};
    long value{0};
    long step{0};
    bool isAuto{false};
    bool autoIsEnable{false};
};

struct CameraDevice {
    int DeviceIndex{-1};
    const char *FriendlyName{};
    const char *MonikerName{};
    IBaseFilter *CaptureFilter{};

    bool operator==(const CameraDevice &cameraInfo) const {
        return DeviceIndex == cameraInfo.DeviceIndex && FriendlyName == cameraInfo.FriendlyName && MonikerName == cameraInfo.MonikerName;
    }
};

namespace UVC_Win_DShow {
    inline char* WCharToChar(const WCHAR* s)
    {
        const int w_len = WideCharToMultiByte(CP_ACP, 0, s, -1, nullptr, 0, nullptr, nullptr);
        const auto ret = new char[w_len];
        memset(ret, 0, w_len);
        WideCharToMultiByte(CP_ACP, 0, s, -1, ret, w_len, nullptr, nullptr);
        return ret;
    }

    int getDevices(std::vector<CameraDevice> &vectorDevices);
    int getResolutions(const CameraDevice& camera, std::vector<DeviceResolution> &vectorResolutions);
    int getParameters(const CameraDevice& camera, std::vector<DeviceParameter> &vectorParameters);
    int getParameter(const CameraDevice& camera, ParameterType property, DeviceParameter &parameter);
    int setParameter(const CameraDevice& camera, ParameterType property, int value, bool isAuto = false);
    int setParameterDefault(const CameraDevice& camera, ParameterType property);

    bool getVideoProcAmpProperty(const CameraDevice& camera, ParameterType property, DeviceParameter &parameter);
    bool setVideoProcAmpProperty(const CameraDevice& camera, ParameterType property, int value,bool isAuto);
    bool getCameraControlProperty(const CameraDevice& camera, ParameterType property, DeviceParameter &parameter);
    bool setCameraControlProperty(const CameraDevice& camera, ParameterType property, int value,bool isAuto);
};

#endif //UVC_WIN_DSHOW_H
