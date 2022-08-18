#include <openvr.h>

#include <Eigen/Dense>

#include <array>
#include <numeric>
#include <vector>
#include <iostream>

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

static std::vector<CalibrationPoint> calibrationPoints{
    {{0.7880420088768005f, -0.0519401989877224f, -0.070656418800354f, -0.4038f, 0.026786452159285545f,
      0.9983839392662048f, 0.9634336829185486f, 1.624f, 0.6150387525558472f, 0.023068254813551903f,
      0.04857129603624344f, 1.575f},
     {-0.23822151124477386f, -0.9708361029624939f, -0.9393746256828308f, 0.7465f, -0.9440230131149292f, 0.22493328154087067f,
      0.2254788726568222f, -0.4782f, -0.22819243371486664f, 0.08296337723731995f, 0.04786209389567375f, -0.4290f}},
    {{0.6885039806365967f, -0.06677759438753128f, -0.04047994688153267f, -0.1487f, 0.06610509753227234f, 0.9973850846290588f,
      0.9651123881340027f, 1.62f, -0.7222137451171875f, 0.027631117030978203f, 0.049747347831726074f, 1.572f},
     {-0.08914639800786972f, -0.966161847114563f, -0.9258884191513062f, 0.8194f, 0.25905701518058777f, 0.21215280890464783f,
      0.2364901900291443f, -0.4775f, -0.9617389440536499f, 0.14670255780220032f, 0.14952509105205536f, -0.4264f}},
    {{-0.7717888951301575f, -0.04351676627993584f, -0.020919926464557648f, -0.09491f, -0.00378628121688962f, 0.9979520440101624f,
      0.9668152928352356f, 1.617f, -0.6358680129051208f, 0.04687642678618431f, 0.019634781405329704f, 1.571f},
     {0.32705622911453247f, -0.8744129538536072f, -0.833319902420044f, 0.8255f, 0.9446520209312439f, 0.31287774443626404f,
      0.2991586923599243f, -0.5083f, -0.02583012916147709f, 0.3708222210407257f, 0.3894062936306f, -0.4526f}},
    {{-0.9530506730079651f, -0.011900623328983784f, -0.021589823067188263f, -0.2336f, -0.05416715145111084f, 0.9898108243942261f,
      0.9612066149711609f, 1.611f, 0.29792648553848267f, 0.14189177751541138f, 0.10569589585065842f, 1.570f},
     {0.296106219291687f, -0.9544697999954224f, -0.9214485883712769f, 0.8403f, 0.22930718958377838f, 0.03427921608090401f,
      0.0007115622283890843f, -0.4810f, 0.9272213578224182f, 0.2963304817676544f, 0.2940867245197296f, -0.4290f}},
    {{0.9989162683486938f, -0.04350392147898674f, -0.04260587692260742f, -0.03791f, 0.0411730520427227f, 0.9917164444923401f,
      0.9546036124229431f, 1.617f, 0.021718477830290794f, 0.12085484713315964f, 0.1499074250459671f, 1.562f},
     {-0.07713522762060165f, -0.9891191124916077f, -0.9519727230072021f, 0.8603f, -0.6054352521896362f, 0.14629699289798737f,
      0.16749820113182068f, -0.5155f, -0.7921484112739563f, -0.015498914755880833f, -0.03532002866268158f, -0.4542f}}
};

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

void record_calibration_point()
{
    std::vector<vr::TrackedDevicePose_t> hmdPoses;
    hmdPoses.reserve(200);
    std::array<vr::TrackedDevicePose_t, 30> headPoses;

    std::array<vr::TrackedDevicePose_t, 13> poses;

    uint16_t gotNewHeadPoses = 0;
    while (gotNewHeadPoses != 30)
    {
        poses.fill(kInitPose);
        vr::VRSystem()->GetDeviceToAbsoluteTrackingPose(vr::ETrackingUniverseOrigin::TrackingUniverseStanding, 0.f,
                                                        poses.data(), static_cast<uint32_t>(poses.size()));
        hmdPoses.push_back(poses[0]);
        for (uint32_t ui = 1; ui < static_cast<uint32_t>(poses.size()); ui++)
        {
            if (poses[ui].bDeviceIsConnected && poses[ui].bPoseIsValid)
            {
                if (IsK4AHeadPose(ui))
                {
                    if (poses[ui].mDeviceToAbsoluteTracking.m[0][3] !=
                        headPoses[gotNewHeadPoses].mDeviceToAbsoluteTracking.m[0][3])
                    {
                        headPoses[gotNewHeadPoses++] = poses[ui];
                    }
                }
                else
                {
                    continue;
                }
            }
        }
    }

    const auto hmdPoseAvg = average_pose_matrix(hmdPoses);
    const auto headPoseAvg = average_pose_matrix<30>(headPoses);

    CalibrationPoint p{hmdPoseAvg, headPoseAvg};
    calibrationPoints.push_back(p);
}

inline size_t factorial(const size_t n)
{
    size_t f = 1;
    for (size_t i = n; i > 0; i--)
        f *= i;
    return f;
}

Eigen::Vector3d axis(const Eigen::Matrix3d m)
{
    return Eigen::Vector3d{m(2, 1) - m(1, 2), m(0, 2) - m(2, 0), m(1, 0) - m(0, 1)};
}

vr::HmdMatrix34_t solve_calibration_matrix()
{
    std::vector<EigenCalibrationPoint> points;
    points.reserve(calibrationPoints.size());

    for (const auto &point : calibrationPoints)
    {
        points.push_back({{Eigen::Matrix3d{{point.hmd.m[0][0], point.hmd.m[0][1], point.hmd.m[0][2]},
                                           {point.hmd.m[1][0], point.hmd.m[1][1], point.hmd.m[1][2]},
                                           {point.hmd.m[2][0], point.hmd.m[2][1], point.hmd.m[2][2]}},
                           Eigen::Vector3d{
                               point.hmd.m[0][3],
                               point.hmd.m[1][3],
                               point.hmd.m[2][3],
                           }},
                          {Eigen::Matrix3d{{point.head.m[0][0], point.head.m[0][1], point.head.m[0][2]},
                                           {point.head.m[1][0], point.head.m[1][1], point.head.m[1][2]},
                                           {point.head.m[2][0], point.head.m[2][1], point.head.m[2][2]}},
                           Eigen::Vector3d{
                               point.head.m[0][3],
                               point.head.m[1][3],
                               point.head.m[2][3],
                           }}});
    }

    const auto pf = factorial(calibrationPoints.size());
    size_t deltaCount = pf / (2 * factorial(calibrationPoints.size()-2));
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
        eye(2,2) = -1;

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
    const auto translateCm = translate * 100.0;

    return vr::HmdMatrix34_t{
        {
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
        }
    };
    //return kInitPose.mDeviceToAbsoluteTracking;
}

int main() {

    const auto matrix = solve_calibration_matrix();

    std::cout << "1" << std::endl;
}