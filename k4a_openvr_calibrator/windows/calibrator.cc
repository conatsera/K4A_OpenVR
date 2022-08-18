#include <openvr.h>

#include <Eigen/Dense>

#include <array>
#include <chrono>
#include <numeric>
#include <vector>

#if defined(_WIN32)
#define DLL_EXPORT extern "C" __declspec(dllexport)
#define DLL_IMPORT extern "C" __declspec(dllimport)
#elif defined(__GNUC__) || defined(COMPILER_GCC) || defined(__APPLE__)
#define DLL_EXPORT extern "C" __attribute__((visibility("default")))
#define DLL_IMPORT extern "C"
#else
#error "Unsupported Platform."
#endif

static vr::IVRSystem *vrSystem = nullptr;

struct CalibrationPoint
{
    vr::HmdMatrix34_t hmd;
    vr::HmdMatrix34_t head;
};

struct EigenCalibrationPoint
{
    std::pair<Eigen::Matrix3d, Eigen::Vector3d> hmd;
    std::pair<Eigen::Matrix3d, Eigen::Vector3d> head;
};

static std::vector<CalibrationPoint> calibrationPoints(0);

DLL_EXPORT bool init_openvr()
{
    if (vrSystem != nullptr)
        return true;

    if (!vr::VR_IsHmdPresent())
    {
        return false;
    }

    auto eLastHmdError = vr::VRInitError_None;
    vrSystem = vr::VR_Init(&eLastHmdError, vr::VRApplication_Overlay);

    if (eLastHmdError != vr::VRInitError_None)
    {
        return false;
    }
    else
    {
        return true;
    }
}

DLL_EXPORT void shutdown_openvr()
{
    vr::VR_Shutdown();
}

constexpr const vr::TrackedDevicePose_t kInitPose{vr::HmdMatrix34_t{{
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
                                                  vr::HmdVector3_t{0.f, 0.f, 0.f},
                                                  vr::HmdVector3_t{0.f, 0.f, 0.f},
                                                  vr::ETrackingResult::TrackingResult_Uninitialized,
                                                  false,
                                                  false};

DLL_EXPORT vr::HmdMatrix34_t get_openvr_hmd_pose()
{
    if (!vr::VR_IsHmdPresent() || vrSystem == nullptr)
    {
        return kInitPose.mDeviceToAbsoluteTracking;
    }

    vr::TrackedDevicePose_t hmdPose{kInitPose};

    bool gotValidPose = false;
    while (!gotValidPose)
    {
        vr::VRSystem()->GetDeviceToAbsoluteTrackingPose(vr::ETrackingUniverseOrigin::TrackingUniverseStanding, 0.f,
                                                        &hmdPose, 1);
        if (hmdPose.bDeviceIsConnected && hmdPose.bPoseIsValid &&
            hmdPose.eTrackingResult == vr::ETrackingResult::TrackingResult_Running_OK)
            gotValidPose = true;
    }

    return hmdPose.mDeviceToAbsoluteTracking;
}

bool IsK4AHeadPose(vr::TrackedDeviceIndex_t i)
{
    vr::ETrackedPropertyError err{vr::ETrackedPropertyError::TrackedProp_Success};

    const auto strSize = vr::VRSystem()->GetStringTrackedDeviceProperty(
        i, vr::ETrackedDeviceProperty::Prop_ModelNumber_String, nullptr, 0, &err);

    if (err != vr::ETrackedPropertyError::TrackedProp_BufferTooSmall || strSize == 0)
        return false;

    char *modelName = (char *)malloc(strSize);

    vr::VRSystem()->GetStringTrackedDeviceProperty(i, vr::ETrackedDeviceProperty::Prop_ModelNumber_String, modelName,
                                                   strSize, &err);

    if (err != vr::ETrackedPropertyError::TrackedProp_Success || std::strcmp(modelName, "K4ABT_JOINT_HEAD") != 0)
    {
        free(modelName);
        return false;
    }
    else
    {
        free(modelName);
        return true;
    }
}

vr::HmdMatrix34_t average_pose_matrix(std::vector<vr::TrackedDevicePose_t> &v)
{
    const auto poseSum =
        std::reduce(v.begin(), v.end(), kInitPose,
                    [](const vr::TrackedDevicePose_t &a, const vr::TrackedDevicePose_t &b) -> vr::TrackedDevicePose_t {
                        return vr::TrackedDevicePose_t{
                            vr::HmdMatrix34_t{{
                                a.mDeviceToAbsoluteTracking.m[0][0] + b.mDeviceToAbsoluteTracking.m[0][0],
                                a.mDeviceToAbsoluteTracking.m[0][1] + b.mDeviceToAbsoluteTracking.m[0][1],
                                a.mDeviceToAbsoluteTracking.m[0][2] + b.mDeviceToAbsoluteTracking.m[0][2],
                                a.mDeviceToAbsoluteTracking.m[0][3] + b.mDeviceToAbsoluteTracking.m[0][3],

                                a.mDeviceToAbsoluteTracking.m[1][0] + b.mDeviceToAbsoluteTracking.m[1][0],
                                a.mDeviceToAbsoluteTracking.m[1][1] + b.mDeviceToAbsoluteTracking.m[1][1],
                                a.mDeviceToAbsoluteTracking.m[1][2] + b.mDeviceToAbsoluteTracking.m[1][2],
                                a.mDeviceToAbsoluteTracking.m[1][3] + b.mDeviceToAbsoluteTracking.m[1][3],

                                a.mDeviceToAbsoluteTracking.m[1][0] + b.mDeviceToAbsoluteTracking.m[2][0],
                                a.mDeviceToAbsoluteTracking.m[1][1] + b.mDeviceToAbsoluteTracking.m[2][1],
                                a.mDeviceToAbsoluteTracking.m[1][2] + b.mDeviceToAbsoluteTracking.m[2][2],
                                a.mDeviceToAbsoluteTracking.m[1][3] + b.mDeviceToAbsoluteTracking.m[2][3],
                            }},
                            vr::HmdVector3_t{{0.f, 0.f, 0.f}},
                            vr::HmdVector3_t{0.f, 0.f, 0.f},
                            vr::ETrackingResult::TrackingResult_Running_OK,
                            true,
                            true};
                    });

    const auto fPoseCount = static_cast<float>(v.size());

    return vr::HmdMatrix34_t{{
        poseSum.mDeviceToAbsoluteTracking.m[0][0] / fPoseCount,
        poseSum.mDeviceToAbsoluteTracking.m[0][1] / fPoseCount,
        poseSum.mDeviceToAbsoluteTracking.m[0][2] / fPoseCount,
        poseSum.mDeviceToAbsoluteTracking.m[0][3] / fPoseCount,

        poseSum.mDeviceToAbsoluteTracking.m[1][0] / fPoseCount,
        poseSum.mDeviceToAbsoluteTracking.m[1][1] / fPoseCount,
        poseSum.mDeviceToAbsoluteTracking.m[1][2] / fPoseCount,
        poseSum.mDeviceToAbsoluteTracking.m[1][3] / fPoseCount,

        poseSum.mDeviceToAbsoluteTracking.m[2][0] / fPoseCount,
        poseSum.mDeviceToAbsoluteTracking.m[2][1] / fPoseCount,
        poseSum.mDeviceToAbsoluteTracking.m[2][2] / fPoseCount,
        poseSum.mDeviceToAbsoluteTracking.m[2][3] / fPoseCount,
    }};
}

template <size_t n> vr::HmdMatrix34_t average_pose_matrix(std::array<vr::TrackedDevicePose_t, n> &a)
{
    const auto poseSum = std::reduce(
        a.begin(), a.end(), kInitPose, [](const vr::TrackedDevicePose_t &a, const vr::TrackedDevicePose_t &b) {
            return vr::TrackedDevicePose_t{
                vr::HmdMatrix34_t{{
                    a.mDeviceToAbsoluteTracking.m[0][0] + b.mDeviceToAbsoluteTracking.m[0][0],
                    a.mDeviceToAbsoluteTracking.m[0][1] + b.mDeviceToAbsoluteTracking.m[0][1],
                    a.mDeviceToAbsoluteTracking.m[0][2] + b.mDeviceToAbsoluteTracking.m[0][2],
                    a.mDeviceToAbsoluteTracking.m[0][3] + b.mDeviceToAbsoluteTracking.m[0][3],

                    a.mDeviceToAbsoluteTracking.m[1][0] + b.mDeviceToAbsoluteTracking.m[1][0],
                    a.mDeviceToAbsoluteTracking.m[1][1] + b.mDeviceToAbsoluteTracking.m[1][1],
                    a.mDeviceToAbsoluteTracking.m[1][2] + b.mDeviceToAbsoluteTracking.m[1][2],
                    a.mDeviceToAbsoluteTracking.m[1][3] + b.mDeviceToAbsoluteTracking.m[1][3],

                    a.mDeviceToAbsoluteTracking.m[1][0] + b.mDeviceToAbsoluteTracking.m[2][0],
                    a.mDeviceToAbsoluteTracking.m[1][1] + b.mDeviceToAbsoluteTracking.m[2][1],
                    a.mDeviceToAbsoluteTracking.m[1][2] + b.mDeviceToAbsoluteTracking.m[2][2],
                    a.mDeviceToAbsoluteTracking.m[1][3] + b.mDeviceToAbsoluteTracking.m[2][3],
                }},
                vr::HmdVector3_t{{0.f, 0.f, 0.f}},
                vr::HmdVector3_t{0.f, 0.f, 0.f},
                vr::ETrackingResult::TrackingResult_Running_OK,
                true,
                true};
        });

    const auto fPoseCount = static_cast<float>(a.size());

    return vr::HmdMatrix34_t{{
        poseSum.mDeviceToAbsoluteTracking.m[0][0] / fPoseCount,
        poseSum.mDeviceToAbsoluteTracking.m[0][1] / fPoseCount,
        poseSum.mDeviceToAbsoluteTracking.m[0][2] / fPoseCount,
        poseSum.mDeviceToAbsoluteTracking.m[0][3] / fPoseCount,

        poseSum.mDeviceToAbsoluteTracking.m[1][0] / fPoseCount,
        poseSum.mDeviceToAbsoluteTracking.m[1][1] / fPoseCount,
        poseSum.mDeviceToAbsoluteTracking.m[1][2] / fPoseCount,
        poseSum.mDeviceToAbsoluteTracking.m[1][3] / fPoseCount,

        poseSum.mDeviceToAbsoluteTracking.m[2][0] / fPoseCount,
        poseSum.mDeviceToAbsoluteTracking.m[2][1] / fPoseCount,
        poseSum.mDeviceToAbsoluteTracking.m[2][2] / fPoseCount,
        poseSum.mDeviceToAbsoluteTracking.m[2][3] / fPoseCount,
    }};
}

DLL_EXPORT vr::HmdMatrix34_t get_openvr_k4a_head_pose()
{
    if (!vr::VR_IsHmdPresent() || vrSystem == nullptr)
    {
        return kInitPose.mDeviceToAbsoluteTracking;
    }
    std::array<vr::TrackedDevicePose_t, 13> poses;
    poses.fill(kInitPose);

    uint32_t gotValidPose = 0;
    while (gotValidPose == 0)
    {
        vr::VRSystem()->GetDeviceToAbsoluteTrackingPose(vr::ETrackingUniverseOrigin::TrackingUniverseStanding, 0.f,
                                                        poses.data(), static_cast<uint32_t>(poses.size()));
        for (uint32_t ui = 1; ui < static_cast<uint32_t>(poses.size()); ui++)
        {
            if (poses[ui].bDeviceIsConnected && poses[ui].bPoseIsValid)
            {
                if (IsK4AHeadPose(ui))
                {
                    return poses[ui].mDeviceToAbsoluteTracking;
                }
                else
                {
                    continue;
                }
            }
        }
    }

    if (gotValidPose)
        return poses[gotValidPose].mDeviceToAbsoluteTracking;
    else
        return kInitPose.mDeviceToAbsoluteTracking;
}

DLL_EXPORT CalibrationPoint record_calibration_point()
{
    std::vector<vr::TrackedDevicePose_t> hmdPoses;
    hmdPoses.reserve(1000);
    std::vector<vr::TrackedDevicePose_t> headPoses;
    headPoses.reserve(1000);

    std::array<vr::TrackedDevicePose_t, 13> poses;

    auto completeTime = std::chrono::steady_clock::now() + std::chrono::seconds(5);
    while (completeTime > std::chrono::steady_clock::now())
    {
        poses.fill(kInitPose);
        vr::VRSystem()->GetDeviceToAbsoluteTrackingPose(vr::ETrackingUniverseOrigin::TrackingUniverseStanding, 0.f,
                                                        poses.data(), static_cast<uint32_t>(poses.size()));

        for (uint32_t ui = 1; ui < static_cast<uint32_t>(poses.size()); ui++)
        {
            if (poses[ui].bDeviceIsConnected && poses[ui].bPoseIsValid)
            {
                if (IsK4AHeadPose(ui))
                {
                    hmdPoses.push_back(poses[0]);
                    headPoses.push_back(poses[ui]);
                }
                else
                {
                    continue;
                }
            }
        }
    }

    const auto hmdPoseAvg = average_pose_matrix(hmdPoses);
    const auto headPoseAvg = average_pose_matrix(headPoses);

    CalibrationPoint p{hmdPoseAvg, headPoseAvg};
    calibrationPoints.push_back(p);
    return p;
}

DLL_EXPORT void reset_calibration_points()
{
    calibrationPoints.clear();
}

DLL_EXPORT uint64_t get_calibration_point_count()
{
    return static_cast<uint64_t>(calibrationPoints.size());
}

DLL_EXPORT CalibrationPoint get_calibration_point(uint16_t i)
{
    return calibrationPoints.at(static_cast<size_t>(i));
}

static vr::HmdMatrix34_t sCalibrationMatrix{kInitPose.mDeviceToAbsoluteTracking};

DLL_EXPORT vr::HmdMatrix34_t get_calibration_matrix()
{
    return sCalibrationMatrix;
}

inline size_t factorial(const size_t n)
{
    size_t f = 1;
    for (size_t i = n; i > 0; i--)
        f *= i;
    return f;
}

inline Eigen::Vector3d axis(const Eigen::Matrix3d m)
{
    return Eigen::Vector3d{m(2, 1) - m(1, 2), m(0, 2) - m(2, 0), m(1, 0) - m(0, 1)};
}

DLL_EXPORT vr::HmdMatrix34_t solve_calibration_matrix()
{
    std::vector<EigenCalibrationPoint> points;
    points.reserve(calibrationPoints.size());

    for (const auto &point : calibrationPoints)
    {
        points.push_back({{
                              Eigen::Matrix3d{
                                  {point.hmd.m[0][0], point.hmd.m[0][1], point.hmd.m[0][2]},
                                  {point.hmd.m[1][0], point.hmd.m[1][1], point.hmd.m[1][2]},
                                  {point.hmd.m[2][0], point.hmd.m[2][1], point.hmd.m[2][2]},
                              },
                              Eigen::Vector3d{
                                  point.hmd.m[0][3],
                                  point.hmd.m[1][3],
                                  point.hmd.m[2][3],
                              },
                          },
                          {
                              Eigen::Matrix3d{
                                  {point.head.m[0][0], point.head.m[0][1], point.head.m[0][2]},
                                  {point.head.m[1][0], point.head.m[1][1], point.head.m[1][2]},
                                  {point.head.m[2][0], point.head.m[2][1], point.head.m[2][2]},
                              },
                              Eigen::Vector3d{
                                  point.head.m[0][3],
                                  point.head.m[1][3],
                                  point.head.m[2][3],
                              },
                          }});
    }

    const auto pf = factorial(calibrationPoints.size());
    size_t deltaCount = pf / (2 * factorial(calibrationPoints.size() - 2));
    Eigen::MatrixXd hmdRotDeltas(deltaCount, 3), headRotDeltas(deltaCount, 3);
    Eigen::Vector3d hmdCentroid{0.0, 0.0, 0.0}, headCentroid{0.0, 0.0, 0.0};
    std::vector<std::pair<Eigen::Vector3d, Eigen::Matrix3d>> posDeltas;
    posDeltas.reserve(deltaCount * 2);

    size_t c = 0;
    for (size_t i = 0; i < calibrationPoints.size(); i++)
    {
        for (size_t j = 0; j < i; j++)
        {
            const auto hmdRotIT = points[i].hmd.first.transpose();
            const auto hmdRotJT = points[j].hmd.first.transpose();
            const auto headRotIT = points[i].head.first.transpose();
            const auto headRotJT = points[j].head.first.transpose();

            const auto hmdRotDiff = hmdRotJT - hmdRotIT;
            const auto headRotDiff = headRotJT - headRotIT;

            const auto hmdPosDiff = hmdRotJT * (points[j].hmd.second - points[j].head.second) -
                                    hmdRotIT * (points[i].hmd.second - points[i].head.second);
            const auto headPosDiff = headRotJT * (points[j].hmd.second - points[j].head.second) -
                                     headRotIT * (points[i].hmd.second - points[i].head.second);

            posDeltas.push_back({hmdPosDiff, hmdRotDiff});
            posDeltas.push_back({headPosDiff, headRotDiff});

            const auto hmdAxisDiff = axis(points[i].hmd.first * hmdRotJT).normalized();
            const auto headAxisDiff = axis(points[i].head.first * headRotJT).normalized();

            hmdRotDeltas.row(c) = hmdAxisDiff;
            headRotDeltas.row(c++) = headAxisDiff;

            hmdCentroid += hmdAxisDiff;
            headCentroid += headAxisDiff;
        }
    }

    hmdCentroid /= static_cast<double>(deltaCount);
    headCentroid /= static_cast<double>(deltaCount);

    for (size_t i = 0; i < deltaCount; i++)
    {
        hmdRotDeltas.row(i) -= hmdCentroid;
        headRotDeltas.row(i) -= headCentroid;
    }

    const auto crossCV = hmdRotDeltas.transpose() * headRotDeltas;

    Eigen::BDCSVD<Eigen::MatrixXd> bdcsvd;
    const auto svd = bdcsvd.compute(crossCV, Eigen::ComputeThinU | Eigen::ComputeThinV);

    Eigen::Matrix3d eye = Eigen::Matrix3d::Identity();
    if ((svd.matrixU() * svd.matrixV().transpose()).determinant() < 0)
        eye(2, 2) = -1;

    Eigen::Matrix3d rot = svd.matrixV() * eye * svd.matrixU().transpose();
    rot.transposeInPlace();

    Eigen::VectorXd constants(deltaCount * 2 * 3);
    Eigen::MatrixXd coefficients(deltaCount * 2 * 3, 3);

    for (size_t i = 0; i < deltaCount * 2; i++)
    {
        for (size_t axis = 0; axis < 3; axis++)
        {
            constants(i * 3 + axis) = posDeltas[i].first(axis);
            coefficients.row(i * 3 + axis) = posDeltas[i].second.row(axis);
        }
    }

    Eigen::Vector3d translate = coefficients.bdcSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(constants);

    return vr::HmdMatrix34_t{{
        static_cast<float>(rot(0, 0)),
        static_cast<float>(rot(0, 1)),
        static_cast<float>(rot(0, 2)),
        static_cast<float>(translate(0)),
        static_cast<float>(rot(1, 0)),
        static_cast<float>(rot(1, 1)),
        static_cast<float>(rot(1, 2)),
        static_cast<float>(translate(1)),
        static_cast<float>(rot(2, 0)),
        static_cast<float>(rot(2, 1)),
        static_cast<float>(rot(2, 2)),
        static_cast<float>(translate(2)),
    }};

    // return kInitPose.mDeviceToAbsoluteTracking;
}