#if defined(_WIN32)
#define HMD_DLL_EXPORT extern "C" __declspec(dllexport)
#define HMD_DLL_IMPORT extern "C" __declspec(dllimport)
#elif defined(__GNUC__) || defined(COMPILER_GCC) || defined(__APPLE__)
#define HMD_DLL_EXPORT extern "C" __attribute__((visibility("default")))
#define HMD_DLL_IMPORT extern "C"
#else
#error "Unsupported Platform."
#endif

#include "driver.h"

//K4AOpenVR::WatchdogProvider g_watchdogProvider;
K4AOpenVR::TrackedJointProvider g_trackedJointProvider;

HMD_DLL_EXPORT void *HmdDriverFactory(const char *pInterfaceName, int *pReturnCode)
{
    if (pReturnCode)
        *pReturnCode = vr::EVRInitError::VRInitError_None;
    if (0 == strcmp(vr::IServerTrackedDeviceProvider_Version, pInterfaceName))
    {
        return &g_trackedJointProvider;
    }
    /* if (0 == strcmp(vr::IVRWatchdogProvider_Version, pInterfaceName))
    {
        return &g_watchdogProvider;
    }*/

    if (pReturnCode)
        *pReturnCode = vr::EVRInitError::VRInitError_Init_InterfaceNotFound;

    return NULL;
}