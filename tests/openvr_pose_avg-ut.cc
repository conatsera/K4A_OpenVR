#include <future>
#include <array>
#include <numeric>
#include <iostream>

#include <openvr.h>

inline vr::HmdQuaternion_t GetRotation(vr::HmdMatrix34_t hmdMatrix)
{
    vr::HmdQuaternion_t q;
    q.w = std::sqrt(std::fmax(0, 1 + hmdMatrix.m[0][0] + hmdMatrix.m[1][1] + hmdMatrix.m[2][2])) / 2;
    q.x = std::sqrt(std::fmax(0, 1 + hmdMatrix.m[0][0] - hmdMatrix.m[1][1] - hmdMatrix.m[2][2])) / 2;
    q.y = std::sqrt(std::fmax(0, 1 - hmdMatrix.m[0][0] + hmdMatrix.m[1][1] - hmdMatrix.m[2][2])) / 2;
    q.z = std::sqrt(std::fmax(0, 1 - hmdMatrix.m[0][0] - hmdMatrix.m[1][1] + hmdMatrix.m[2][2])) / 2;
    q.x = _copysign(q.x, hmdMatrix.m[2][1] - hmdMatrix.m[1][2]);
    q.y = _copysign(q.y, hmdMatrix.m[0][2] - hmdMatrix.m[2][0]);
    q.z = _copysign(q.z, hmdMatrix.m[1][0] - hmdMatrix.m[0][1]);
    return q;
}

int main()
{
    bool finishAverage = false;

    auto eLastHmdError = vr::VRInitError_None;
    vr::VR_Init(&eLastHmdError, vr::VRApplication_Overlay);

    if (eLastHmdError != vr::VRInitError_None)
    {
        std::cout << eLastHmdError << std::endl;
        return -1;
    }

    auto hmdPoseAverageFuture =
        // Sample rate of HMDs is indeterminate, assume 250Hz as a baseline
        std::async(std::launch::async, [&]() -> vr::TrackedDevicePose_t {
            std::array<vr::TrackedDevicePose_t, 5> hmdPoses;
            hmdPoses.fill(vr::TrackedDevicePose_t{vr::HmdMatrix34_t{{
                                                      0.f,
                                                      0.f,
                                                      0.f,
                                                      0.f,
                                                      0.f,
                                                      0.f,
                                                      0.f,
                                                      0.f,
                                                      0.f,
                                                      0.f,
                                                      0.f,
                                                      0.f,
                                                  }},
                                                  vr::HmdVector3_t{0.f, 0.f, 0.f}, vr::HmdVector3_t{0.f, 0.f, 0.f},
                                                  vr::ETrackingResult::TrackingResult_Uninitialized, false, false});

            uint16_t ui = 0;
            while (!finishAverage && ui != 5)
            {
                vr::VRSystem()->GetDeviceToAbsoluteTrackingPose(
                    vr::ETrackingUniverseOrigin::TrackingUniverseStanding, 0.f, &hmdPoses[ui++], 1);
            }

            if (ui == 0)
                return vr::TrackedDevicePose_t{
                    vr::HmdMatrix34_t{{0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f}},
                     vr::HmdVector3_t{},
                     vr::HmdVector3_t{},
                     vr::ETrackingResult::TrackingResult_Uninitialized,
                     false,
                     false
                        };

            const auto sumPose = std::reduce(
                &hmdPoses[0], &hmdPoses[ui - 1],
                vr::TrackedDevicePose_t{vr::HmdMatrix34_t{{
                                            0.f,
                                            0.f,
                                            0.f,
                                            0.f,
                                            0.f,
                                            0.f,
                                            0.f,
                                            0.f,
                                            0.f,
                                            0.f,
                                            0.f,
                                            0.f,
                                        }},
                                        vr::HmdVector3_t{}, vr::HmdVector3_t{},
                                        vr::ETrackingResult::TrackingResult_Running_OK, true, true},
                [](const auto &poseA, const auto &poseB) -> vr::TrackedDevicePose_t {
                    if (poseA.eTrackingResult != vr::ETrackingResult::TrackingResult_Running_OK)
                        return poseB;
                    else if (poseB.eTrackingResult != vr::ETrackingResult::TrackingResult_Running_OK)
                        return poseA;
                    else
                    {
                        auto pose = vr::TrackedDevicePose_t{vr::HmdMatrix34_t{},
                                                            vr::HmdVector3_t{},
                                                            vr::HmdVector3_t{},
                                                            vr::ETrackingResult::TrackingResult_Running_OK,
                                                            true,
                                                            true};
                        pose.mDeviceToAbsoluteTracking.m[0][0] =
                            poseA.mDeviceToAbsoluteTracking.m[0][0] + poseB.mDeviceToAbsoluteTracking.m[0][0];
                        pose.mDeviceToAbsoluteTracking.m[0][1] =
                            poseA.mDeviceToAbsoluteTracking.m[0][1] + poseB.mDeviceToAbsoluteTracking.m[0][1];
                        pose.mDeviceToAbsoluteTracking.m[0][2] =
                            poseA.mDeviceToAbsoluteTracking.m[0][2] + poseB.mDeviceToAbsoluteTracking.m[0][2];
                        pose.mDeviceToAbsoluteTracking.m[0][3] =
                            poseA.mDeviceToAbsoluteTracking.m[0][3] + poseB.mDeviceToAbsoluteTracking.m[0][3];

                        pose.mDeviceToAbsoluteTracking.m[1][0] =
                            poseA.mDeviceToAbsoluteTracking.m[1][0] + poseB.mDeviceToAbsoluteTracking.m[1][0];
                        pose.mDeviceToAbsoluteTracking.m[1][1] =
                            poseA.mDeviceToAbsoluteTracking.m[1][1] + poseB.mDeviceToAbsoluteTracking.m[1][1];
                        pose.mDeviceToAbsoluteTracking.m[1][2] =
                            poseA.mDeviceToAbsoluteTracking.m[1][2] + poseB.mDeviceToAbsoluteTracking.m[1][2];
                        pose.mDeviceToAbsoluteTracking.m[1][3] =
                            poseA.mDeviceToAbsoluteTracking.m[1][3] + poseB.mDeviceToAbsoluteTracking.m[1][3];

                        pose.mDeviceToAbsoluteTracking.m[2][0] =
                            poseA.mDeviceToAbsoluteTracking.m[2][0] + poseB.mDeviceToAbsoluteTracking.m[2][0];
                        pose.mDeviceToAbsoluteTracking.m[2][1] =
                            poseA.mDeviceToAbsoluteTracking.m[2][1] + poseB.mDeviceToAbsoluteTracking.m[2][1];
                        pose.mDeviceToAbsoluteTracking.m[2][2] =
                            poseA.mDeviceToAbsoluteTracking.m[2][2] + poseB.mDeviceToAbsoluteTracking.m[2][2];
                        pose.mDeviceToAbsoluteTracking.m[2][3] =
                            poseA.mDeviceToAbsoluteTracking.m[2][3] + poseB.mDeviceToAbsoluteTracking.m[2][3];
                        return pose;
                    }
                });

            vr::TrackedDevicePose_t averagePose{vr::HmdMatrix34_t{},
                                                vr::HmdVector3_t{},
                                                vr::HmdVector3_t{},
                                                vr::ETrackingResult::TrackingResult_Running_OK,
                                                true,
                                                true};

            averagePose.mDeviceToAbsoluteTracking.m[0][0] = sumPose.mDeviceToAbsoluteTracking.m[0][0] / ui;
            averagePose.mDeviceToAbsoluteTracking.m[0][1] = sumPose.mDeviceToAbsoluteTracking.m[0][1] / ui;
            averagePose.mDeviceToAbsoluteTracking.m[0][2] = sumPose.mDeviceToAbsoluteTracking.m[0][2] / ui;
            averagePose.mDeviceToAbsoluteTracking.m[0][3] = sumPose.mDeviceToAbsoluteTracking.m[0][3] / ui;

            averagePose.mDeviceToAbsoluteTracking.m[1][0] = sumPose.mDeviceToAbsoluteTracking.m[1][0] / ui;
            averagePose.mDeviceToAbsoluteTracking.m[1][1] = sumPose.mDeviceToAbsoluteTracking.m[1][1] / ui;
            averagePose.mDeviceToAbsoluteTracking.m[1][2] = sumPose.mDeviceToAbsoluteTracking.m[1][2] / ui;
            averagePose.mDeviceToAbsoluteTracking.m[1][3] = sumPose.mDeviceToAbsoluteTracking.m[1][3] / ui;

            averagePose.mDeviceToAbsoluteTracking.m[2][0] = sumPose.mDeviceToAbsoluteTracking.m[2][0] / ui;
            averagePose.mDeviceToAbsoluteTracking.m[2][1] = sumPose.mDeviceToAbsoluteTracking.m[2][1] / ui;
            averagePose.mDeviceToAbsoluteTracking.m[2][2] = sumPose.mDeviceToAbsoluteTracking.m[2][2] / ui;
            averagePose.mDeviceToAbsoluteTracking.m[2][3] = sumPose.mDeviceToAbsoluteTracking.m[2][3] / ui;

            return averagePose;
        });

    hmdPoseAverageFuture.wait();

    const auto averagePose = hmdPoseAverageFuture.get();

    std::cout << "{ " << averagePose.mDeviceToAbsoluteTracking.m[0][3] << ", "
              << averagePose.mDeviceToAbsoluteTracking.m[1][3] << ", " << averagePose.mDeviceToAbsoluteTracking.m[2][3]
              << " }" << std::endl;
}