//
// Created by liu_zongchang on 2025/1/8 4:01.
// Email 1439797751@qq.com
// 
//

#include "UVC_Win_DShow.h"

#include "FileTools/Microscope_Utils_Log.h"

int UVC_Win_DShow::getDevices(std::vector<CameraDevice>& vectorDevices)
{
    if (!vectorDevices.empty())
    {
        vectorDevices.clear();
    }

    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    ICreateDevEnum *pSysDevEnum = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, nullptr, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum,
                          reinterpret_cast<void**>(&pSysDevEnum));
    if (FAILED(hr))
    {
        pSysDevEnum = nullptr;
        LOG_ERROR("CoCreateInstance failed :{}", hr);
        CoUninitialize();
        return false;
    }

    IEnumMoniker *pEnumCat = nullptr;
    hr = pSysDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnumCat, 0);
    if (FAILED(hr))
    {
        LOG_ERROR("CreateClassEnumerator failed :{}", hr);
        pSysDevEnum->Release();
        CoUninitialize();
        return false;
    }

    if (pEnumCat == nullptr)
    {
        LOG_ERROR("Camera device not found");
        pSysDevEnum->Release();
        CoUninitialize();
        return false;
    }
    pEnumCat->Reset();


    //枚举设备
    IMoniker* pMoniker = nullptr;
    ULONG cFetched;
    int index = 0;
    while (pEnumCat->Next(1, &pMoniker, &cFetched) == S_OK)
    {
        IPropertyBag* pPropBag;
        hr = pMoniker->BindToStorage(nullptr, nullptr, IID_IPropertyBag, reinterpret_cast<void**>(&pPropBag));
        if (FAILED(hr))
        {
            LOG_ERROR("BindToStorage failed :{}", hr);
            pMoniker->Release();
            continue;
        }

        VARIANT varName;
        VariantInit(&varName);
        hr = pPropBag->Read(L"FriendlyName", &varName, nullptr);
        if (FAILED(hr))
        {
            LOG_ERROR("Read FriendlyName failed :{}", hr);
            VariantClear(&varName);
            pPropBag->Release();
            pMoniker->Release();
            continue;
        }

        CameraDevice camInfo;
        camInfo.DeviceIndex = index;
        camInfo.FriendlyName = WCharToChar(varName.bstrVal);
        VariantClear(&varName);
        auto pOleDisplayName = static_cast<LPOLESTR>(CoTaskMemAlloc(512));

        if (pOleDisplayName == nullptr)
        {
            LOG_ERROR("CoTaskMemAlloc failed");
            pPropBag->Release();
            pMoniker->Release();
            continue;
        }

        hr = pMoniker->GetDisplayName(nullptr, nullptr, &pOleDisplayName);
        if (FAILED(hr))
        {
            LOG_ERROR("GetDisplayName failed :{}", hr);
            CoTaskMemFree(pOleDisplayName);
            pPropBag->Release();
            pMoniker->Release();
            continue;
        }

        camInfo.MonikerName = WCharToChar(pOleDisplayName);
        CoTaskMemFree(pOleDisplayName);

        hr = pMoniker->BindToObject(nullptr, nullptr, IID_IBaseFilter, reinterpret_cast<void**>(&camInfo.CaptureFilter));
        if (FAILED(hr))
        {
            LOG_ERROR("BindToObject failed :{}", hr);
            pPropBag->Release();
            pMoniker->Release();
            continue;
        }


        vectorDevices.push_back(camInfo);
        pPropBag->Release();
        pMoniker->Release();
        index++;
    }
    return true;
}

int UVC_Win_DShow::getResolutions(const CameraDevice& camera, std::vector<DeviceResolution>& vectorResolutions)
{

    if (!vectorResolutions.empty())
    {
        vectorResolutions.clear();
    }

    if (camera.CaptureFilter == nullptr)
    {
        LOG_ERROR("CaptureFilter is nullptr");
        return false;
    }

    IEnumPins* pinEnum = nullptr;

    if (FAILED(camera.CaptureFilter->EnumPins(&pinEnum)))
    {
        LOG_ERROR("EnumPins failed");
        pinEnum->Release();
        return false;
    }
    pinEnum->Reset();


    ULONG pinFetched = 0;
    IPin* pin = nullptr;
    while (SUCCEEDED(pinEnum->Next(1, &pin, &pinFetched)) && pinFetched)
    {
        if (pin == nullptr)
        {
            LOG_WARN("pin is nullptr");
            continue;
        }

        PIN_INFO pinInfo;
        if (FAILED(pin->QueryPinInfo(&pinInfo)))
        {
            LOG_WARN("QueryPinInfo failed");
            continue;
        }

        if (pinInfo.dir != PINDIR_OUTPUT)
        {
            LOG_WARN("dir is not PINDIR_OUTPUT");
            continue;
        }

        IEnumMediaTypes *mtEnum = nullptr;
        if (FAILED(pin->EnumMediaTypes(&mtEnum)))
        {
            LOG_ERROR("EnumMediaTypes failed");
            break;
        }
        mtEnum->Reset();

        ULONG mtFetched = 0;
        AM_MEDIA_TYPE *mt = nullptr;
        while (SUCCEEDED(mtEnum->Next(1, &mt, &mtFetched)) && mtFetched)
        {
            if (mt->formattype == FORMAT_VideoInfo)
            {
                if (mt->cbFormat >= sizeof(VIDEOINFOHEADER))
                {
                    const auto *pVih = reinterpret_cast<VIDEOINFOHEADER*>(mt->pbFormat);
                    const BITMAPINFOHEADER* pHeader = &pVih->bmiHeader;
                    const auto avgTime = static_cast<double>(pVih->AvgTimePerFrame);
                    if (pHeader != nullptr)
                    {
                        DeviceResolution resolution;
                        resolution.width = pHeader->biWidth;
                        resolution.height = pHeader->biHeight;
                        resolution.fps = 10000000 / avgTime;
                        if (MediaSubTypes.find(mt->subtype) != MediaSubTypes.end())
                        {
                            resolution.mediaSubType = mt->subtype;
                            vectorResolutions.push_back(resolution);
                        } else {
                            SetConsoleOutputCP(CP_UTF8);
                            OLECHAR* guidString;
                            StringFromCLSID(mt->subtype, &guidString);
                            LOG_DEBUG("MediaSubTypes not found :{}", WCharToChar(guidString));
                            CoTaskMemFree(guidString);
                        }
                    }
                }
            }
        }
        pin->Release();
    }

    std::sort(vectorResolutions.begin(), vectorResolutions.end(), [](const DeviceResolution& a, const DeviceResolution& b) {
        if (a.width == b.width)
        {
            if (a.height == b.height)
            {
                return a.fps > b.fps;
            }
            return a.height > b.height;
        }
        return a.width > b.width;
    });
    return true;
}

int UVC_Win_DShow::getParameters(const CameraDevice& camera, std::vector<DeviceParameter>& vectorParameters)
{

    if (!vectorParameters.empty())
    {
        vectorParameters.clear();
    }

    if (camera.CaptureFilter == nullptr)
    {
        LOG_ERROR("CaptureFilter is nullptr");
        return false;
    }

    for (int i = Brightness; i <= Focus; ++i)
    {
        DeviceParameter parameter;
        if (!getParameter(camera, static_cast<ParameterType>(i), parameter))
        {
            LOG_WARN("getParameter failed :{}", i);
            continue;
        }
        vectorParameters.push_back(parameter);
    }

    if (vectorParameters.empty())
    {
        LOG_WARN("vectorParameters is empty");
        return false;
    }

    return true;
}

int UVC_Win_DShow::getParameter(const CameraDevice& camera, const ParameterType property, DeviceParameter& parameter)
{
    if (camera.CaptureFilter == nullptr)
    {
        LOG_ERROR("CaptureFilter is nullptr");
        return false;
    }

    if (property < Brightness || property > Focus)
    {
        LOG_ERROR("type is invalid");
        return false;
    }

    if (property < Pan)
    {
        return getVideoProcAmpProperty(camera, property, parameter);
    }

    return getCameraControlProperty(camera, property, parameter);
}

int UVC_Win_DShow::setParameter(const CameraDevice& camera, const ParameterType property, const int value, const bool isAuto)
{
    if (camera.CaptureFilter == nullptr)
    {
        LOG_ERROR("CaptureFilter is nullptr");
        return false;
    }

    if (property < Brightness || property > Focus)
    {
        LOG_ERROR("type is invalid");
        return false;
    }

    if (property < Pan)
    {
        return setVideoProcAmpProperty(camera, property, value, isAuto);
    }

    return setCameraControlProperty(camera, property, value, isAuto);
}

int UVC_Win_DShow::setParameterDefault(const CameraDevice& camera, const ParameterType property)
{

    if (camera.CaptureFilter == nullptr)
    {
        LOG_ERROR("CaptureFilter is nullptr");
        return false;
    }

    if (property < Brightness || property > Focus)
    {
        LOG_ERROR("type is invalid");
        return false;
    }

    IAMVideoProcAmp* pVideoProcAmp = nullptr;
    HRESULT hr = camera.CaptureFilter->QueryInterface(IID_IAMVideoProcAmp, reinterpret_cast<void**>(&pVideoProcAmp));
    if (SUCCEEDED(hr)) {
        for (int i = 0; i < 11; ++i) {
            long Min, Max, Step, Default, Flags;
            hr = pVideoProcAmp->GetRange(i, &Min, &Max, &Step, &Default, &Flags);
            if (SUCCEEDED(hr)) {
                hr = pVideoProcAmp->Set(i, Default, Flags);
                if (FAILED(hr)) {
                    LOG_ERROR("set failed :{}", hr);
                }
            } else if (hr == E_PROP_ID_UNSUPPORTED) {
                LOG_WARN("UnSupport this property");
            } else {
                LOG_ERROR("GetRange failed :{}", hr);
            }
        }
    }
    pVideoProcAmp->Release();

    IAMCameraControl* pCameraControl = nullptr;
    hr = camera.CaptureFilter->QueryInterface(IID_IAMCameraControl, reinterpret_cast<void**>(&pCameraControl));
    if (SUCCEEDED(hr)) {
        for (int i = 0; i < 11; ++i) {
            long Min, Max, Step, Default, Flags;
            hr = pVideoProcAmp->GetRange(i, &Min, &Max, &Step, &Default, &Flags);
            if (SUCCEEDED(hr)) {
                hr = pVideoProcAmp->Set(i, Default, Flags);
                if (FAILED(hr)) {
                    LOG_ERROR("set failed :{}", hr);
                }
            } else if (hr == E_PROP_ID_UNSUPPORTED) {
                LOG_WARN("UnSupport this property");
            } else {
                LOG_ERROR("GetRange failed :{}", hr);
            }
        }
    }
    pCameraControl->Release();
    return true;
}

bool UVC_Win_DShow::getVideoProcAmpProperty(const CameraDevice& camera, const ParameterType property, DeviceParameter& parameter)
{
    if (camera.CaptureFilter == nullptr)
    {
        LOG_ERROR("CaptureFilter is nullptr");
        return false;
    }

    if (property < Brightness || property > Gain)
    {
        LOG_ERROR("property is invalid");
        return false;
    }

    parameter.dataType = property;

    IAMVideoProcAmp* pVideoProcAmp = nullptr;
    HRESULT hr = camera.CaptureFilter->QueryInterface(IID_IAMVideoProcAmp, reinterpret_cast<void**>(&pVideoProcAmp));
    if (FAILED(hr))
    {
        LOG_ERROR("QueryInterface failed");
        return false;
    }

    long Min, Max, Step, Default, Flags;
    hr = pVideoProcAmp->GetRange(property, &Min, &Max, &Step, &Default, &Flags);
    if (FAILED(hr))
    {
        LOG_ERROR("GetRange failed :{}", hr);
        pVideoProcAmp->Release();
        return false;
    }

    parameter.autoIsEnable = false;
    if(Flags == 3){
        parameter.autoIsEnable = true;
    }

    long Val;
    hr = pVideoProcAmp->Get(property, &Val, &Flags);
    if (FAILED(hr))
    {
        LOG_ERROR("Get failed :{}", hr);
        pVideoProcAmp->Release();
        return false;
    }

    parameter.min = Min;
    parameter.max = Max;
    parameter.def = Default;
    parameter.value = Val;
    parameter.step = Step;
    if(Flags & VideoProcAmp_Flags_Auto){
        parameter.isAuto = true;
    }else{
        parameter.isAuto = false;
    }

    pVideoProcAmp->Release();
    return true;
}

bool UVC_Win_DShow::setVideoProcAmpProperty(const CameraDevice& camera, const ParameterType property, const int value, const bool isAuto)
{
    if (camera.CaptureFilter == nullptr)
    {
        LOG_ERROR("CaptureFilter is nullptr");
        return false;
    }

    if (property < Brightness || property > Gain)
    {
        LOG_ERROR("property is invalid");
        return false;
    }


    //设置参数
    IAMVideoProcAmp* pVideoProcAmp = nullptr;
    if (HRESULT hr = camera.CaptureFilter->QueryInterface(IID_IAMVideoProcAmp, reinterpret_cast<void**>(&pVideoProcAmp)); SUCCEEDED(hr))
    {
        long Min, Max, Step, Default, Flags;
        hr = pVideoProcAmp->GetRange(property, &Min, &Max, &Step, &Default, &Flags);
        if (SUCCEEDED(hr))
        {
            if (value < Min || value > Max)
            {
                LOG_WARN("value out of range");
                pVideoProcAmp->Release();
                return false;
            }

            if(isAuto){
                hr = pVideoProcAmp->Set(property, value, VideoProcAmp_Flags_Auto);
            }
            else{
                hr = pVideoProcAmp->Set(property, value, VideoProcAmp_Flags_Manual);
            }

            if (SUCCEEDED(hr))
            {
                LOG_DEBUG("set {} success, value = {}, auto = {}", ParameterTypes.at(property), value, isAuto);
            }
            else{
                LOG_ERROR("set failed :{}", hr);
            }
        }
        else if(hr == E_PROP_ID_UNSUPPORTED){
            LOG_WARN("UnSupport this property");
        }
        else{
            LOG_ERROR("GetRange failed :{}", hr);
        }
    }
    return true;
}

bool UVC_Win_DShow::getCameraControlProperty(const CameraDevice& camera, const ParameterType property,
                                             DeviceParameter& parameter)
{

    if (camera.CaptureFilter == nullptr)
    {
        LOG_ERROR("CaptureFilter is nullptr");
        return false;
    }

    if (property < Pan || property > Focus)
    {
        LOG_ERROR("property is invalid");
        return false;
    }
    parameter.dataType = property;

    IAMCameraControl* pCameraControl = nullptr;
    HRESULT hr = camera.CaptureFilter->QueryInterface(IID_IAMCameraControl, reinterpret_cast<void**>(&pCameraControl));
    if (FAILED(hr))
    {
        LOG_ERROR("QueryInterface failed");
        return false;
    }

    long Min, Max, Step, Default, Flags;
    hr = pCameraControl->GetRange(property - 10, &Min, &Max, &Step, &Default, &Flags);
    if (FAILED(hr))
    {
        LOG_ERROR("GetRange failed :{}", hr);
        pCameraControl->Release();
        return false;
    }

    parameter.autoIsEnable = false;
    if(Flags == 3){
        parameter.autoIsEnable = true;
    }

    long Val;
    hr = pCameraControl->Get(property - 10, &Val, &Flags);
    if (FAILED(hr))
    {
        LOG_ERROR("Get failed :{}", hr);
        pCameraControl->Release();
        return false;
    }

    parameter.min = Min;
    parameter.max = Max;
    parameter.def = Default;
    parameter.value = Val;
    parameter.step = Step;
    if(Flags & VideoProcAmp_Flags_Auto){
        parameter.isAuto = true;
    }else{
        parameter.isAuto = false;
    }

    pCameraControl->Release();
    return true;
}

bool UVC_Win_DShow::setCameraControlProperty(const CameraDevice& camera, const ParameterType property, const int value, const bool isAuto)
{
    if (camera.CaptureFilter == nullptr)
    {
        LOG_ERROR("CaptureFilter is nullptr");
        return false;
    }

    if (property < Pan || property > Focus)
    {
        LOG_ERROR("property is invalid");
        return false;
    }

    IAMCameraControl* pCameraControl = nullptr;
    if (HRESULT hr = camera.CaptureFilter->QueryInterface(IID_IAMCameraControl, reinterpret_cast<void**>(&pCameraControl)); SUCCEEDED(hr))
    {
        long Min, Max, Step, Default, Flags;
        hr = pCameraControl->GetRange(property  - 10, &Min, &Max, &Step, &Default, &Flags);
        if (SUCCEEDED(hr))
        {
            if (value < Min || value > Max)
            {
                LOG_WARN("value out of range");
                pCameraControl->Release();
                return false;
            }

            if(isAuto){
                hr = pCameraControl->Set(property - 10, value, VideoProcAmp_Flags_Auto);
            }
            else{
                hr = pCameraControl->Set(property - 10, value, VideoProcAmp_Flags_Manual);
            }

            if (SUCCEEDED(hr))
            {
                LOG_DEBUG("set {} success, value = {}, auto = {}", ParameterTypes.at(property), value, isAuto);
            }
            else{
                LOG_ERROR("set failed :{}", hr);
            }
        }
        else if(hr == E_PROP_ID_UNSUPPORTED){
            LOG_WARN("UnSupport this property");
        }
        else{
            LOG_ERROR("GetRange failed :{}", hr);
        }
    }
    return true;
}
