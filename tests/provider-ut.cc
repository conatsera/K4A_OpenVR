#include "driver.h"

#include <iostream>

class MockWwatchdogHost : public vr::IVRWatchdogHost
{
  public:
    virtual void WatchdogWakeUp(vr::ETrackedDeviceClass eDeviceClass) override
    {
        std::cout << "wake up received: " << eDeviceClass << std::endl;
    }
};

class MockSettings : public vr::IVRSettings
{
  public:
    virtual const char *GetSettingsErrorNameFromEnum(vr::EVRSettingsError eError)
    {
        return "EVRSettingsError";
    }

    virtual void SetBool(const char* pchSection, const char* pchSettingsKey, bool bValue,
        vr::EVRSettingsError* peError = nullptr) {

    }
    virtual void SetInt32(const char *pchSection, const char *pchSettingsKey, int32_t nValue,
                          vr::EVRSettingsError *peError = nullptr)
    {
    }
    virtual void SetFloat(const char *pchSection, const char *pchSettingsKey, float flValue,
                          vr::EVRSettingsError *peError = nullptr)
    {
    }
    virtual void SetString(const char *pchSection, const char *pchSettingsKey, const char *pchValue,
                           vr::EVRSettingsError *peError = nullptr)
    {
    }

    // Users of the system need to provide a proper default in default.vrsettings in the resources/settings/ directory
    // of either the runtime or the driver_xxx directory. Otherwise the default will be false, 0, 0.0 or ""
    virtual bool GetBool(const char *pchSection, const char *pchSettingsKey, vr::EVRSettingsError *peError = nullptr)
    {
        return false;
    }
    virtual int32_t GetInt32(const char *pchSection, const char *pchSettingsKey,
                             vr::EVRSettingsError *peError = nullptr)
    {
        return 0;
    }
    virtual float GetFloat(const char *pchSection, const char *pchSettingsKey, vr::EVRSettingsError *peError = nullptr)
    {
        return 0.f;
    }
    virtual void GetString(const char *pchSection, const char *pchSettingsKey, VR_OUT_STRING() char *pchValue,
                           uint32_t unValueLen, vr::EVRSettingsError *peError = nullptr)
    {
    }

    virtual void RemoveSection(const char *pchSection, vr::EVRSettingsError *peError = nullptr)
    {
    }
    virtual void RemoveKeyInSection(const char *pchSection, const char *pchSettingsKey,
                                    vr::EVRSettingsError *peError = nullptr)
    {
    }
};

class MockDriverLog : public vr::IVRDriverLog
{
    virtual void Log(const char* pchLogMessage) {
        std::cout << pchLogMessage;
    }
};

class MockServerDriverHost : public vr::IVRServerDriverHost
{
    virtual bool TrackedDeviceAdded(const char* pchDeviceSerialNumber, vr::ETrackedDeviceClass eDeviceClass,
        vr::ITrackedDeviceServerDriver* pDriver) {
        pDriver->Activate(ids++);
        return true;
    }

    virtual void TrackedDevicePoseUpdated(uint32_t unWhichDevice, const vr::DriverPose_t &newPose,
                                          uint32_t unPoseStructSize){}

    virtual void VsyncEvent(double vsyncTimeOffsetSeconds) {}

    virtual void VendorSpecificEvent(uint32_t unWhichDevice, vr::EVREventType eventType,
                                     const vr::VREvent_Data_t &eventData, double eventTimeOffset) {}

    virtual bool IsExiting()
    {
        return false;
    }

    virtual bool PollNextEvent(vr::VREvent_t *pEvent, uint32_t uncbVREvent)
    {
        return false;
    }

    virtual void GetRawTrackedDevicePoses(float fPredictedSecondsFromNow,
                                          vr::TrackedDevicePose_t *pTrackedDevicePoseArray,
                                          uint32_t unTrackedDevicePoseArrayCount) {}

    virtual void RequestRestart(const char *pchLocalizedReason, const char *pchExecutableToStart,
                                const char *pchArguments, const char *pchWorkingDirectory) {}

    virtual uint32_t GetFrameTimings(vr::Compositor_FrameTiming *pTiming, uint32_t nFrames)
    {
        return 1;
    }

    virtual void SetDisplayEyeToHead(uint32_t unWhichDevice, const vr::HmdMatrix34_t &eyeToHeadLeft,
                                     const vr::HmdMatrix34_t &eyeToHeadRight) {}

    virtual void SetDisplayProjectionRaw(uint32_t unWhichDevice, const vr::HmdRect2_t &eyeLeft,
                                         const vr::HmdRect2_t &eyeRight) {}

    virtual void SetRecommendedRenderTargetSize(uint32_t unWhichDevice, uint32_t nWidth, uint32_t nHeight) {}

  private:
    uint32_t ids = 1;
};

class MockProperties : public vr::IVRProperties
{
    virtual vr::ETrackedPropertyError ReadPropertyBatch(vr::PropertyContainerHandle_t ulContainerHandle,
                                                        vr::PropertyRead_t *pBatch, uint32_t unBatchEntryCount)
    {
        return vr::ETrackedPropertyError::TrackedProp_Success;
    }

    virtual vr::ETrackedPropertyError WritePropertyBatch(vr::PropertyContainerHandle_t ulContainerHandle,
                                                         vr::PropertyWrite_t *pBatch, uint32_t unBatchEntryCount)
    {
        return vr::ETrackedPropertyError::TrackedProp_Success;
    }

    virtual const char *GetPropErrorNameFromEnum(vr::ETrackedPropertyError error)
    {
        return "ETrackedPropertyError";
    }

    virtual vr::PropertyContainerHandle_t TrackedDeviceToPropertyContainer(vr::TrackedDeviceIndex_t nDevice) {
        return vr::k_ulInvalidPropertyContainer;
    };
};

class MockDriverManager : public vr::IVRDriverManager
{
    virtual uint32_t GetDriverCount() const
    {
        return 1;
    }

    /** Returns the length of the number of bytes necessary to hold this string including the trailing null. */
    virtual uint32_t GetDriverName(vr::DriverId_t nDriver, VR_OUT_STRING() char *pchValue, uint32_t unBufferSize)
    {
        return 1;
    }

    virtual vr::DriverHandle_t GetDriverHandle(const char *pchDriverName)
    {
        return vr::k_ulInvalidDriverHandle;
    }

    virtual bool IsEnabled(vr::DriverId_t nDriver) const
    {
        return true;
    }
};

class MockResources : public vr::IVRResources
{
    virtual uint32_t LoadSharedResource(const char *pchResourceName, char *pchBuffer, uint32_t unBufferLen)
    {
        return 0;
    }

    virtual uint32_t GetResourceFullPath(const char *pchResourceName, const char *pchResourceTypeDirectory,
                                         VR_OUT_STRING() char *pchPathBuffer, uint32_t unBufferLen)
    {
        return 0;
    }
};

class MockDriverContext : public vr::IVRDriverContext
{
    virtual void* GetGenericInterface(const char* pchInterfaceVersion, vr::EVRInitError* peError = nullptr) {
        *peError = vr::EVRInitError::VRInitError_None;
        if (std::strcmp("IVRWatchdogHost_002", pchInterfaceVersion) == 0)
            return &m_mockWatchdogHost;
        else if (std::strcmp("IVRSettings_003", pchInterfaceVersion) == 0)
            return &m_mockSettings;
        else if (std::strcmp("IVRDriverLog_001", pchInterfaceVersion) == 0)
            return &m_mockDriverLog;
        else if (std::strcmp("IVRServerDriverHost_006", pchInterfaceVersion) == 0)
            return &m_mockServerDriverHost;
        else if (std::strcmp("IVRProperties_001", pchInterfaceVersion) == 0)
            return &m_mockProperties;
        else if (std::strcmp("IVRDriverManager_001", pchInterfaceVersion) == 0)
            return &m_mockDriverManager;
        else if (std::strcmp("IVRResources_001", pchInterfaceVersion) == 0)
            return &m_mockResources;
        *peError = vr::EVRInitError::VRInitError_Init_InterfaceNotFound;
        return nullptr;
    };

    /** Returns the property container handle for this driver */
    virtual vr::DriverHandle_t GetDriverHandle() {
        return vr::k_ulInvalidPropertyContainer;
    };

    MockWwatchdogHost m_mockWatchdogHost;
    MockSettings m_mockSettings;
    MockDriverLog m_mockDriverLog;
    MockServerDriverHost m_mockServerDriverHost;
    MockProperties m_mockProperties;
    MockDriverManager m_mockDriverManager;
    MockResources m_mockResources;
};

K4AOpenVR::WatchdogProvider g_watchdogProvider;
K4AOpenVR::TrackedJointProvider g_trackedJointProvider;

int main()
{
    MockDriverContext driverContext;
    auto err = g_watchdogProvider.Init(&driverContext);

    if (err != vr::EVRInitError::VRInitError_None)
    {
        std::cout << "failed watchdog init" << std::endl;
    }

    err = g_trackedJointProvider.Init(&driverContext);

    if (err != vr::EVRInitError::VRInitError_None)
    {
        std::cout << "failed tracked joint provider init" << std::endl;
    }

    int getC = 0;
    
    while (getC == 0)
    {
        getC = std::cin.get();
        std::chrono::time_point<std::chrono::steady_clock> tp;
        do
            std::this_thread::yield();
        while (std::chrono::steady_clock::now() < tp);
        tp = std::chrono::steady_clock::now() + std::chrono::milliseconds(100);
    }

    g_trackedJointProvider.Cleanup();

    g_watchdogProvider.Cleanup();
}