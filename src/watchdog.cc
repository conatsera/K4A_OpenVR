#include "driver.h"

namespace K4AOpenVR
{

vr::EVRInitError WatchdogProvider::Init(vr::IVRDriverContext *pDriverContext)
{
    VR_INIT_WATCHDOG_DRIVER_CONTEXT(pDriverContext);

    InitDriverLog(vr::VRDriverLog());

    m_pWatchdogThread = std::thread([&] {
        std::vector<vr::TrackedDevicePose_t> trackedPoses(vr::k_unMaxTrackedDeviceCount);
        while (!m_exiting)
        {
            std::this_thread::sleep_for(std::chrono::seconds(5));

            vr::VRWatchdogHost()->WatchdogWakeUp(vr::TrackedDeviceClass_GenericTracker);
        }
    });

    if (!m_pWatchdogThread.joinable())
    {
        DriverLog("Failed to create watchdog thread\n");
        return vr::EVRInitError::VRInitError_Driver_Failed;
    }

    return vr::EVRInitError::VRInitError_None;
}

void WatchdogProvider::Cleanup()
{
    m_exiting = true;

    if (m_pWatchdogThread.joinable())
        m_pWatchdogThread.join();

    CleanupDriverLog();
}

} // namespace K4AOpenVR