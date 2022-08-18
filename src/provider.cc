#include "driver.h"

#include "driverlog.h"

#include <future>
#include <numeric>

namespace K4AOpenVR
{

static void k4a_log_cb(void *context, k4a_log_level_t level, const char *file, const int line, const char *message)
{
    DriverLog("%d : %s:%d - %s\n", level, file, line, message);
}

grpc::Status TrackedJointProvider::Get(grpc::ServerContext *context, const calibration::SettingsGetRequest *request,
                                       calibration::SettingsUpdateRequest *reply)
{
    reply->set_lite(m_k4abtLiteMode);
    reply->set_smoothing(m_k4abtSmoothingFactor);

    reply->set_mode(
        static_cast<calibration::SettingsUpdateRequest_ProcessingMode>(m_k4abtTrackerConfig.processing_mode));

    reply->set_depth_res(static_cast<calibration::SettingsUpdateRequest_DepthResolution>(m_k4aConfig.depth_mode - 1));

    return grpc::Status::OK;
}

grpc::Status TrackedJointProvider::Update(grpc::ServerContext *context,
                                          const calibration::SettingsUpdateRequest *request,
                                          calibration::SettingsReply *reply)
{
    reply->set_success(true);
    DriverLog("Lite: %i Mode: %s Depth Res: %s Smoothing: %f", request->lite(),
              request->ProcessingMode_Name(request->mode()).c_str(),
              request->DepthResolution_Name(request->depth_res()).c_str(), request->smoothing());

    m_k4aConfig.depth_mode = static_cast<k4a_depth_mode_t>(request->depth_res() + 1);
    m_k4abtTrackerConfig.processing_mode = static_cast<k4abt_tracker_processing_mode_t>(request->mode());
    m_k4abtSmoothingFactor = request->smoothing();
    m_k4abtLiteMode = request->lite();

    RestartTracking();

    return grpc::Status::OK;
}

grpc::Status TrackedJointProvider::PauseResume(grpc::ServerContext *context,
                                               const calibration::PauseResumeRequest *request,
                                               calibration::PauseResumeReply *reply)
{
    m_paused = !m_paused;
    reply->set_paused(m_paused);
    return grpc::Status::OK;
}

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

inline float QuaternionDotProduct(const k4a_quaternion_t &quata, const k4a_quaternion_t &quatb)
{
    return ((quata.wxyz.x * quatb.wxyz.x) + (quata.wxyz.y * quatb.wxyz.y)) +
           ((quata.wxyz.z * quatb.wxyz.z) + (quata.wxyz.w * quatb.wxyz.w));
}

inline k4a_quaternion_t QuaternionProduct(k4a_quaternion_t &quata, k4a_quaternion_t &quatb)
{
    k4a_quaternion_t quat;
    quat.wxyz.w = ((quata.wxyz.w * quatb.wxyz.w) - (quata.wxyz.x * quatb.wxyz.x) - (quata.wxyz.y * quatb.wxyz.y) -
                   (quata.wxyz.z * quatb.wxyz.z));
    quat.wxyz.x = ((quata.wxyz.w * quatb.wxyz.x) + (quata.wxyz.x * quatb.wxyz.w) + (quata.wxyz.y * quatb.wxyz.z) -
                   (quata.wxyz.z * quatb.wxyz.y));
    quat.wxyz.y = ((quata.wxyz.w * quatb.wxyz.y) - (quata.wxyz.x * quatb.wxyz.z) + (quata.wxyz.y * quatb.wxyz.w) +
                   (quata.wxyz.z * quatb.wxyz.x));
    quat.wxyz.z = ((quata.wxyz.w * quatb.wxyz.z) + (quata.wxyz.x * quatb.wxyz.y) - (quata.wxyz.y * quatb.wxyz.x) +
                   (quata.wxyz.z * quatb.wxyz.w));
    return quat;
}

inline k4a_quaternion_t QuaternionInverse(k4a_quaternion_t &quat)
{
    k4a_quaternion_t inverse_quat;
    inverse_quat.wxyz.w = quat.wxyz.w;
    inverse_quat.wxyz.x = -(quat.wxyz.x);
    inverse_quat.wxyz.y = -(quat.wxyz.y);
    inverse_quat.wxyz.z = -(quat.wxyz.z);
    return inverse_quat;
}

inline void QuaternionInverseSign(k4a_quaternion_t &quat)
{
    quat.wxyz.w = -(quat.wxyz.w);
    quat.wxyz.x = -(quat.wxyz.x);
    quat.wxyz.y = -(quat.wxyz.y);
    quat.wxyz.z = -(quat.wxyz.z);
}

grpc::Status TrackedJointProvider::GetCalibrationData(grpc::ServerContext *context,
                                                      const calibration::CalibrationDataRequest *request,
                                                      calibration::CalibrationDataReply *reply)
{
    m_calibrationDataCount = 0;

    bool finishAverage = false;

    auto imuAverageFuture = std::async(std::launch::async, [&]() -> k4a_imu_sample_t {
        std::array<k4a_imu_sample_t, 208> samples;

        uint16_t ui = 0;
        while (!finishAverage && ui != 208)
            if (m_k4aDevice.get_imu_sample(&samples[ui], std::chrono::milliseconds(10)))
                ui++;

        const auto sumSample = std::reduce(
            samples.begin(), samples.end(),
            k4a_imu_sample_t{0.f, k4a_float3_t{0.f, 0.f, 0.f}, 0, k4a_float3_t{0.f, 0.f, 0.f}, 0},
            [](const auto &sampleA, const auto &sampleB) -> k4a_imu_sample_t {
                if (sampleA.acc_timestamp_usec == 0)
                    return sampleB;
                else if (sampleB.acc_timestamp_usec == 0)
                    return sampleA;
                else
                    return k4a_imu_sample_t{sampleA.temperature + sampleB.temperature,
                                            k4a_float3_t{sampleA.acc_sample.xyz.x + sampleB.acc_sample.xyz.x,
                                                         sampleA.acc_sample.xyz.y + sampleB.acc_sample.xyz.y,
                                                         sampleA.acc_sample.xyz.z + sampleB.acc_sample.xyz.z},
                                            1, // acc_timestamp_usec set as a validity marker in partial sums
                                            k4a_float3_t{sampleA.gyro_sample.xyz.x + sampleB.gyro_sample.xyz.x,
                                                         sampleA.gyro_sample.xyz.y + sampleB.gyro_sample.xyz.y,
                                                         sampleA.gyro_sample.xyz.z + sampleB.gyro_sample.xyz.z},
                                            0};
            });

        const auto last = ui - 1;

        const k4a_imu_sample_t averagedSample{
            sumSample.temperature / ui,
            k4a_float3_t{sumSample.acc_sample.xyz.x / ui, sumSample.acc_sample.xyz.y / ui,
                         sumSample.acc_sample.xyz.z / ui},
            samples[last].acc_timestamp_usec,
            k4a_float3_t{sumSample.gyro_sample.xyz.x / ui, sumSample.gyro_sample.xyz.y / ui,
                         sumSample.gyro_sample.xyz.z / ui},
            samples[last].gyro_timestamp_usec};

        return averagedSample;
    });

    auto hmdPoseAverageFuture =
        // Sample rate of HMDs is indeterminate, assume 250Hz as a baseline
        std::async(std::launch::async, [&]() -> std::pair<vr::HmdVector3_t, vr::HmdQuaternion_t> {
            std::array<vr::TrackedDevicePose_t, 250> hmdPoses;
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
            while (!finishAverage && ui != 250)
            {
                vr::VRServerDriverHost()->GetRawTrackedDevicePoses(0.f, &hmdPoses[ui++], 1);
                std::this_thread::sleep_for(std::chrono::milliseconds(4));
            }

            if (ui == 0)
                return {vr::HmdVector3_t{
                            0.f,
                            0.f,
                            0.f,
                        },
                        vr::HmdQuaternion_t{1.f, 0.f, 0.f, 0.f}};

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

            return {vr::HmdVector3_t{
                        averagePose.mDeviceToAbsoluteTracking.m[0][3],
                        averagePose.mDeviceToAbsoluteTracking.m[1][3],
                        averagePose.mDeviceToAbsoluteTracking.m[2][3],
                    },
                    GetRotation(averagePose.mDeviceToAbsoluteTracking)};
        });

    const auto timeout = std::chrono::steady_clock::now() + std::chrono::seconds(4);
    while (m_calibrationDataCount != m_calibrationDataMaxCount && std::chrono::steady_clock::now() < timeout)
        std::this_thread::yield();

    const auto calibrationDataCount = m_calibrationDataCount;
    const auto firstQuat = m_calibrationPoses[0].orientation;

    if (calibrationDataCount == 0)
    {
        reply->set_success(false);
        return grpc::Status::OK;
    }

    const auto headJointPoseSum =
        std::reduce(&m_calibrationPoses[1], &m_calibrationPoses[calibrationDataCount - 1], m_calibrationPoses[0],
                    [firstQuat](k4abt_joint_t poseA, k4abt_joint_t poseB) -> k4abt_joint_t {
                        // Bit of a cheat, but the accumulator is identified by confidence level none.
                        // The tracker thread will never supply a no confidence pose for calibration
                        if (poseA.confidence_level == k4abt_joint_confidence_level_t::K4ABT_JOINT_CONFIDENCE_NONE)
                        {
                            // Assume a substantially different orientation is just an inversed sign equivalent. TODO:
                            // Return error if the user moves too much
                            if (QuaternionDotProduct(firstQuat, poseB.orientation) > 0.1f)
                                QuaternionInverseSign(poseB.orientation);

                            return k4abt_joint_t{k4a_float3_t{
                                                     poseA.position.xyz.x + poseB.position.xyz.x,
                                                     poseA.position.xyz.y + poseB.position.xyz.y,
                                                     poseA.position.xyz.z + poseB.position.xyz.z,
                                                 },
                                                 k4a_quaternion_t{
                                                     poseA.orientation.wxyz.w + poseA.orientation.wxyz.w,
                                                     poseA.orientation.wxyz.x + poseA.orientation.wxyz.x,
                                                     poseA.orientation.wxyz.y + poseA.orientation.wxyz.y,
                                                     poseA.orientation.wxyz.z + poseA.orientation.wxyz.z,
                                                 },
                                                 k4abt_joint_confidence_level_t::K4ABT_JOINT_CONFIDENCE_NONE};
                        }
                        else if (poseB.confidence_level == k4abt_joint_confidence_level_t::K4ABT_JOINT_CONFIDENCE_NONE)
                        {
                            // Assume a substantially different orientation is just an inversed sign equivalent. TODO:
                            // Return error if the user moves too much
                            if (QuaternionDotProduct(firstQuat, poseA.orientation) > 0.1f)
                                QuaternionInverseSign(poseA.orientation);

                            return k4abt_joint_t{k4a_float3_t{
                                                     poseA.position.xyz.x + poseB.position.xyz.x,
                                                     poseA.position.xyz.y + poseB.position.xyz.y,
                                                     poseA.position.xyz.z + poseB.position.xyz.z,
                                                 },
                                                 k4a_quaternion_t{
                                                     poseA.orientation.wxyz.w + poseA.orientation.wxyz.w,
                                                     poseA.orientation.wxyz.x + poseA.orientation.wxyz.x,
                                                     poseA.orientation.wxyz.y + poseA.orientation.wxyz.y,
                                                     poseA.orientation.wxyz.z + poseA.orientation.wxyz.z,
                                                 },
                                                 k4abt_joint_confidence_level_t::K4ABT_JOINT_CONFIDENCE_NONE};
                        }
                        else
                            return k4abt_joint_t{k4a_float3_t{
                                                     poseA.position.xyz.x + poseB.position.xyz.x,
                                                     poseA.position.xyz.y + poseB.position.xyz.y,
                                                     poseA.position.xyz.z + poseB.position.xyz.z,
                                                 },
                                                 k4a_quaternion_t{
                                                     poseA.orientation.wxyz.w + poseA.orientation.wxyz.w,
                                                     poseA.orientation.wxyz.x + poseA.orientation.wxyz.x,
                                                     poseA.orientation.wxyz.y + poseA.orientation.wxyz.y,
                                                     poseA.orientation.wxyz.z + poseA.orientation.wxyz.z,
                                                 },
                                                 k4abt_joint_confidence_level_t::K4ABT_JOINT_CONFIDENCE_NONE};
                    });

    const auto headJointPoseAvg = k4abt_joint_t{k4a_float3_t{
                                                    (headJointPoseSum.position.xyz.x / calibrationDataCount) / 1000.f,
                                                    (headJointPoseSum.position.xyz.y / calibrationDataCount) / 1000.f,
                                                    (headJointPoseSum.position.xyz.z / calibrationDataCount) / 1000.f,
                                                },
                                                k4a_quaternion_t{
                                                    headJointPoseSum.orientation.wxyz.w / calibrationDataCount,
                                                    headJointPoseSum.orientation.wxyz.x / calibrationDataCount,
                                                    headJointPoseSum.orientation.wxyz.y / calibrationDataCount,
                                                    headJointPoseSum.orientation.wxyz.z / calibrationDataCount,
                                                },
                                                k4abt_joint_confidence_level_t::K4ABT_JOINT_CONFIDENCE_MEDIUM};

    auto headPos = reply->mutable_data()->mutable_head()->mutable_pose()->mutable_pos();
    headPos->set_x(headJointPoseAvg.position.xyz.x);
    headPos->set_y(headJointPoseAvg.position.xyz.y);
    headPos->set_z(headJointPoseAvg.position.xyz.z);

    auto headRot = reply->mutable_data()->mutable_head()->mutable_pose()->mutable_rot();
    headRot->set_w(headJointPoseAvg.orientation.wxyz.w);
    headRot->set_x(headJointPoseAvg.orientation.wxyz.x);
    headRot->set_y(headJointPoseAvg.orientation.wxyz.y);
    headRot->set_z(headJointPoseAvg.orientation.wxyz.z);

    finishAverage = true;
    imuAverageFuture.wait();
    const auto imuAverage = imuAverageFuture.get();

    auto accSample = reply->mutable_acc()->mutable_sample();
    accSample->set_x(imuAverage.acc_sample.xyz.x);
    accSample->set_y(imuAverage.acc_sample.xyz.y);
    accSample->set_z(imuAverage.acc_sample.xyz.z);

    reply->mutable_acc()->set_ts(imuAverage.acc_timestamp_usec);

    auto gyroSample = reply->mutable_gyro()->mutable_sample();
    gyroSample->set_x(imuAverage.gyro_sample.xyz.x);
    gyroSample->set_y(imuAverage.gyro_sample.xyz.y);
    gyroSample->set_z(imuAverage.gyro_sample.xyz.z);

    reply->mutable_gyro()->set_ts(imuAverage.gyro_timestamp_usec);

    hmdPoseAverageFuture.wait();
    const auto hmdPoseAverage = hmdPoseAverageFuture.get();

    /*auto hmdPosePos = reply->mutable_data()->mutable_hmd()->mutable_pose()->mutable_pos();
    hmdPosePos->set_x(hmdPoseAverage.first.v[0]);
    hmdPosePos->set_y(hmdPoseAverage.first.v[1]);
    hmdPosePos->set_z(hmdPoseAverage.first.v[2]);
    auto hmdPoseQuat = reply->mutable_data()->mutable_hmd()->mutable_pose()->mutable_rot();
    hmdPoseQuat->set_w(hmdPoseAverage.second.w);
    hmdPoseQuat->set_x(hmdPoseAverage.second.x);
    hmdPoseQuat->set_y(hmdPoseAverage.second.y);
    hmdPoseQuat->set_z(hmdPoseAverage.second.z);*/

    if (m_calibrationDataCount != m_calibrationDataMaxCount)
    {
        reply->set_success(false);
        return grpc::Status::OK;
    }

    reply->set_success(true);

    return grpc::Status::OK;
}

grpc::Status TrackedJointProvider::UpdateCalibration(grpc::ServerContext *context,
                                                     const calibration::UpdateCalibrationRequest *request,
                                                     calibration::UpdateCalibrationReply *reply)
{
    m_qWorldFromDriverRotation.w = request->q_world_from_driver_rotation().w();
    m_qWorldFromDriverRotation.x = request->q_world_from_driver_rotation().x();
    m_qWorldFromDriverRotation.y = request->q_world_from_driver_rotation().y();
    m_qWorldFromDriverRotation.z = request->q_world_from_driver_rotation().z();

    m_vecWorldFromDriverTranslation[0] = request->vec_world_from_driver_translation().x();
    m_vecWorldFromDriverTranslation[1] = request->vec_world_from_driver_translation().y();
    m_vecWorldFromDriverTranslation[2] = request->vec_world_from_driver_translation().z();

    m_calibrationUpdateReady = true;
    return grpc::Status::OK;
}

grpc::Status TrackedJointProvider::GetPoseStream(grpc::ServerContext *context,
                                                 const calibration::CalibrationDataRequest *request,
                                                 grpc::ServerWriter<::calibration::CalibrationData> *writer)
{
    writer->SendInitialMetadata();
    m_poseStreamActive = true;

    bool finishAverage = false;
    bool &poseStreamActive = m_poseStreamActive;

    while (m_poseStreamActive)
    {
        finishAverage = false;
        auto hmdPoseAverageFuture =
            // Sample rate of HMDs is indeterminate, assume 250Hz as a baseline
            std::async(std::launch::async, [&finishAverage, &poseStreamActive]() -> vr::TrackedDevicePose_t {
                std::array<vr::TrackedDevicePose_t, 16> hmdPoses;
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
                while (!finishAverage && ui != 16 && poseStreamActive)
                {
                    vr::VRServerDriverHost()->GetRawTrackedDevicePoses(0.f, &hmdPoses[ui], 1);
                    if (hmdPoses[ui].bDeviceIsConnected && hmdPoses[ui].bPoseIsValid &&
                        hmdPoses[ui].eTrackingResult == vr::ETrackingResult::TrackingResult_Running_OK)
                        ui++;
                }

                if (ui == 0)
                    return vr::TrackedDevicePose_t{
                        vr::HmdMatrix34_t{{0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f}},
                        vr::HmdVector3_t{},
                        vr::HmdVector3_t{},
                        vr::ETrackingResult::TrackingResult_Uninitialized,
                        false,
                        false};

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
        auto reply = calibration::CalibrationData().New();

        auto headPos = reply->mutable_head()->mutable_pose()->mutable_pos();
        auto headRot = reply->mutable_head()->mutable_pose()->mutable_rot();

        while (!m_headJointPoseReady && m_poseStreamActive)
            std::this_thread::yield();

        headPos->set_x(m_headJointPose.position.xyz.x / 1000.f);
        headPos->set_y(m_headJointPose.position.xyz.y / 1000.f);
        headPos->set_z(m_headJointPose.position.xyz.z / 1000.f);

        headRot->set_w(m_headJointPose.orientation.wxyz.w);
        headRot->set_x(m_headJointPose.orientation.wxyz.x);
        headRot->set_y(m_headJointPose.orientation.wxyz.y);
        headRot->set_z(m_headJointPose.orientation.wxyz.z);

        m_headJointPoseReady = false;

        finishAverage = true;

        hmdPoseAverageFuture.wait();

        const auto hmdPose = hmdPoseAverageFuture.get();

        auto hmdPosePos = reply->mutable_hmd()->mutable_pose();
        hmdPosePos->set_m00(hmdPose.mDeviceToAbsoluteTracking.m[0][0]);
        hmdPosePos->set_m01(hmdPose.mDeviceToAbsoluteTracking.m[0][1]);
        hmdPosePos->set_m02(hmdPose.mDeviceToAbsoluteTracking.m[0][2]);
        hmdPosePos->set_m03(hmdPose.mDeviceToAbsoluteTracking.m[0][3]);

        hmdPosePos->set_m10(hmdPose.mDeviceToAbsoluteTracking.m[1][0]);
        hmdPosePos->set_m11(hmdPose.mDeviceToAbsoluteTracking.m[1][1]);
        hmdPosePos->set_m12(hmdPose.mDeviceToAbsoluteTracking.m[1][2]);
        hmdPosePos->set_m13(hmdPose.mDeviceToAbsoluteTracking.m[1][3]);

        hmdPosePos->set_m20(hmdPose.mDeviceToAbsoluteTracking.m[2][0]);
        hmdPosePos->set_m21(hmdPose.mDeviceToAbsoluteTracking.m[2][1]);
        hmdPosePos->set_m22(hmdPose.mDeviceToAbsoluteTracking.m[2][2]);
        hmdPosePos->set_m23(hmdPose.mDeviceToAbsoluteTracking.m[2][3]);

        writer->Write(*reply);

        reply->Clear();
        delete reply;
    }

    return grpc::Status::OK;
}

grpc::Status TrackedJointProvider::StopPoseStream(grpc::ServerContext *context,
                                                  const calibration::CalibrationDataRequest *request,
                                                  calibration::UpdateCalibrationReply *reply)
{
    m_poseStreamActive = false;
    reply->set_success(true);
    return grpc::Status::OK;
}

vr::EVRInitError TrackedJointProvider::Init(vr::IVRDriverContext *pDriverContext)
{
    VR_INIT_SERVER_DRIVER_CONTEXT(pDriverContext);

    InitDriverLog(vr::VRDriverLog());

    m_k4aConfig.depth_mode = k4a_depth_mode_t::K4A_DEPTH_MODE_NFOV_UNBINNED;
    m_k4aConfig.camera_fps = k4a_fps_t::K4A_FRAMES_PER_SECOND_30;

    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();

    const std::string serverAddress{"localhost:51942"};

    grpc::ServerBuilder builder;
    builder.AddListeningPort(serverAddress, grpc::InsecureServerCredentials());
    builder.RegisterService((calibration::Settings::Service *)this);
    builder.RegisterService((calibration::Calibration::Service *)this);

    m_calibratorServer = builder.BuildAndStart();

    m_calibratorThread = std::thread([&] {
        std::vector<vr::TrackedDevicePose_t> trackedPoses(vr::k_unMaxTrackedDeviceCount);
        while (!m_exiting)
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            vr::VRServerDriverHost()->GetRawTrackedDevicePoses(0.f, trackedPoses.data(),
                                                               static_cast<uint32_t>(trackedPoses.size()));

            uint32_t ui = 0;
            for (const auto &trackedPose : trackedPoses)
            {
                if (trackedPose.bDeviceIsConnected && trackedPose.bPoseIsValid &&
                    trackedPose.eTrackingResult == vr::ETrackingResult::TrackingResult_Running_OK)
                {
                    const auto trackedDeviceProps = vr::VRProperties()->TrackedDeviceToPropertyContainer(ui);
                    vr::ETrackedPropertyError err = vr::ETrackedPropertyError::TrackedProp_Success;
                    const auto trackedDeviceModelName = vr::VRProperties()->GetStringProperty(
                        trackedDeviceProps, vr::ETrackedDeviceProperty::Prop_ModelNumber_String, &err);
                    if (err != vr::ETrackedPropertyError::TrackedProp_Success)
                        DriverLog("Found device, failed to get property. Error: %i", err);
                    //else
                    //    DriverLog("i: %ui Found device: %s", ui, trackedDeviceModelName.c_str());
                }
                ui++;
            }
        }
    });

    m_pelvisJoint = new TrackedJointDriver(k4abt_joint_id_t::K4ABT_JOINT_PELVIS);
    m_elbowLeftJoint = new TrackedJointDriver(k4abt_joint_id_t::K4ABT_JOINT_ELBOW_LEFT);
    m_elbowRightJoint = new TrackedJointDriver(k4abt_joint_id_t::K4ABT_JOINT_ELBOW_RIGHT);
    m_kneeLeftJoint = new TrackedJointDriver(k4abt_joint_id_t::K4ABT_JOINT_KNEE_LEFT);
    m_kneeRightJoint = new TrackedJointDriver(k4abt_joint_id_t::K4ABT_JOINT_KNEE_RIGHT);
    m_footLeftJoint = new TrackedJointDriver(k4abt_joint_id_t::K4ABT_JOINT_FOOT_LEFT);
    m_footRightJoint = new TrackedJointDriver(k4abt_joint_id_t::K4ABT_JOINT_FOOT_RIGHT);

    vr::VRServerDriverHost()->TrackedDeviceAdded(m_pelvisJoint->GetSerialNumber().c_str(),
                                                 vr::TrackedDeviceClass_GenericTracker, m_pelvisJoint);
    vr::VRServerDriverHost()->TrackedDeviceAdded(m_elbowLeftJoint->GetSerialNumber().c_str(),
                                                 vr::TrackedDeviceClass_GenericTracker, m_elbowLeftJoint);
    vr::VRServerDriverHost()->TrackedDeviceAdded(m_elbowRightJoint->GetSerialNumber().c_str(),
                                                 vr::TrackedDeviceClass_GenericTracker, m_elbowRightJoint);
    vr::VRServerDriverHost()->TrackedDeviceAdded(m_kneeLeftJoint->GetSerialNumber().c_str(),
                                                 vr::TrackedDeviceClass_GenericTracker, m_kneeLeftJoint);
    vr::VRServerDriverHost()->TrackedDeviceAdded(m_kneeRightJoint->GetSerialNumber().c_str(),
                                                 vr::TrackedDeviceClass_GenericTracker, m_kneeRightJoint);
    vr::VRServerDriverHost()->TrackedDeviceAdded(m_footLeftJoint->GetSerialNumber().c_str(),
                                                 vr::TrackedDeviceClass_GenericTracker, m_footLeftJoint);
    vr::VRServerDriverHost()->TrackedDeviceAdded(m_footRightJoint->GetSerialNumber().c_str(),
                                                 vr::TrackedDeviceClass_GenericTracker, m_footRightJoint);

    if constexpr (kHandsAndHeadEnabled)
    {
        m_handLeftJoint = new TrackedJointDriver(k4abt_joint_id_t::K4ABT_JOINT_HAND_LEFT);
        m_handRightJoint = new TrackedJointDriver(k4abt_joint_id_t::K4ABT_JOINT_HAND_RIGHT);
        m_headJoint = new TrackedJointDriver(k4abt_joint_id_t::K4ABT_JOINT_HEAD);

        vr::VRServerDriverHost()->TrackedDeviceAdded(m_handLeftJoint->GetSerialNumber().c_str(),
                                                     vr::TrackedDeviceClass_GenericTracker, m_handLeftJoint);
        vr::VRServerDriverHost()->TrackedDeviceAdded(m_handRightJoint->GetSerialNumber().c_str(),
                                                     vr::TrackedDeviceClass_GenericTracker, m_handRightJoint);
        vr::VRServerDriverHost()->TrackedDeviceAdded(m_headJoint->GetSerialNumber().c_str(),
                                                     vr::TrackedDeviceClass_GenericTracker, m_headJoint);
    }

    k4a_set_debug_message_handler(k4a_log_cb, this, k4a_log_level_t::K4A_LOG_LEVEL_ERROR);

    try
    {
        m_k4aDevice = k4a::device::open(K4A_DEVICE_DEFAULT);
    }
    catch (k4a::error &e)
    {
        DriverLog("K4A SDK: %s\n", e.what());
    }

    ConfigureK4A();

    StartTracking();

    return vr::EVRInitError::VRInitError_None;
}

void TrackedJointProvider::ConfigureK4A()
{
    if (m_k4aDevice)
    {
        try
        {
            m_k4aCalibration =
                m_k4aDevice.get_calibration(m_k4aConfig.depth_mode, k4a_color_resolution_t::K4A_COLOR_RESOLUTION_OFF);

            m_k4aDevice.start_cameras(&m_k4aConfig);
            m_k4aDevice.start_imu();
        }
        catch (k4a::error &e)
        {
            DriverLog("K4A SDK: %s\n", e.what());
        }
    }
}

void TrackedJointProvider::Cleanup()
{
    StopTracking();
    m_k4aDevice.stop_cameras();
    m_k4aDevice.close();

    m_exiting = true;
    if (m_calibratorThread.joinable())
        m_calibratorThread.join();

    m_calibratorServer->Shutdown();

    delete m_pelvisJoint;
    delete m_elbowLeftJoint;
    delete m_elbowRightJoint;
    delete m_kneeLeftJoint;
    delete m_footLeftJoint;
    delete m_kneeRightJoint;
    delete m_footRightJoint;
    m_pelvisJoint = nullptr;
    m_elbowLeftJoint = nullptr;
    m_elbowRightJoint = nullptr;
    m_kneeLeftJoint = nullptr;
    m_footLeftJoint = nullptr;
    m_kneeRightJoint = nullptr;
    m_footRightJoint = nullptr;

    if constexpr (kHandsAndHeadEnabled)
    {
        delete m_handLeftJoint;
        delete m_handRightJoint;
        delete m_headJoint;
        m_handLeftJoint = nullptr;
        m_handRightJoint = nullptr;
        m_headJoint = nullptr;
    }

    CleanupDriverLog();
}

bool TrackedJointProvider::DriversActivated() const
{
    if constexpr (kHandsAndHeadEnabled)
        return m_pelvisJoint->IsActivated() && m_elbowLeftJoint->IsActivated() && m_handLeftJoint->IsActivated() &&
               m_elbowRightJoint->IsActivated() && m_handRightJoint->IsActivated() && m_kneeLeftJoint->IsActivated() &&
               m_footLeftJoint->IsActivated() && m_kneeRightJoint->IsActivated() && m_footRightJoint->IsActivated() &&
               m_headJoint->IsActivated();
    else
        return m_pelvisJoint->IsActivated() && m_elbowLeftJoint->IsActivated() && m_elbowRightJoint->IsActivated() &&
               m_kneeLeftJoint->IsActivated() && m_footLeftJoint->IsActivated() && m_kneeRightJoint->IsActivated() &&
               m_footRightJoint->IsActivated();
}

constexpr const size_t kDriverPoseSize = sizeof(vr::DriverPose_t);

inline void UpdatePose(vr::DriverPose_t &driverPose, const k4abt_joint_t &trackerJoint)
{
    driverPose.poseIsValid = true;

    driverPose.qRotation.w = trackerJoint.orientation.wxyz.w;
    driverPose.qRotation.x = trackerJoint.orientation.wxyz.x;
    driverPose.qRotation.y = trackerJoint.orientation.wxyz.y;
    driverPose.qRotation.z = trackerJoint.orientation.wxyz.z;

    driverPose.vecPosition[0] = trackerJoint.position.xyz.x / 1000;
    driverPose.vecPosition[1] = trackerJoint.position.xyz.y / 1000;
    driverPose.vecPosition[2] = trackerJoint.position.xyz.z / 1000;
}

void TrackedJointProvider::UpdatePoseCalibration(vr::DriverPose_t &driverPose)
{
    driverPose.qWorldFromDriverRotation = m_qWorldFromDriverRotation;
    driverPose.vecWorldFromDriverTranslation[0] = m_vecWorldFromDriverTranslation[0];
    driverPose.vecWorldFromDriverTranslation[1] = m_vecWorldFromDriverTranslation[1];
    driverPose.vecWorldFromDriverTranslation[2] = m_vecWorldFromDriverTranslation[2];
}

constexpr const vr::DriverPose_t kDefaultPose{
    0.0,
    vr::HmdQuaternion_t{1.0, 0.0, 0.0, 0.0},
    {0.0, 0.0, 0.0},
    vr::HmdQuaternion_t{1.0, 0.0, 0.0, 0.0},
    {0.0, 0.0, 0.0},
    {0.0, 0.0, 0.0},
    {0.0, 0.0, 0.0},
    {0.0, 0.0, 0.0},
    vr::HmdQuaternion_t{1.0, 0.0, 0.0, 0.0},
    {0.0, 0.0, 0.0},
    {0.0, 0.0, 0.0},
    vr::ETrackingResult::TrackingResult_Uninitialized,
    false,
    false,
    false,
    false};

void TrackedJointProvider::StartTracking()
{
    m_trackingActive = true;
    m_trackingThread = std::thread([&] {
        while (!DriversActivated() && m_trackingActive)
        {
            std::chrono::time_point<std::chrono::steady_clock> tp;
            do
                std::this_thread::yield();
            while (std::chrono::steady_clock::now() < tp);
            tp = std::chrono::steady_clock::now() + std::chrono::milliseconds(1);
        }

        const auto pelvisId = m_pelvisJoint->GetIndex();
        const auto elbowLeftId = m_elbowLeftJoint->GetIndex();
        const auto elbowRightId = m_elbowRightJoint->GetIndex();
        const auto kneeLeftId = m_kneeLeftJoint->GetIndex();
        const auto footLeftId = m_footLeftJoint->GetIndex();
        const auto kneeRightId = m_kneeRightJoint->GetIndex();
        const auto footRightId = m_footRightJoint->GetIndex();

        const auto handLeftId = kHandsAndHeadEnabled ? m_handLeftJoint->GetIndex() : vr::k_unTrackedDeviceIndexInvalid;
        const auto handRightId =
            kHandsAndHeadEnabled ? m_handRightJoint->GetIndex() : vr::k_unTrackedDeviceIndexInvalid;
        const auto headId = kHandsAndHeadEnabled ? m_headJoint->GetIndex() : vr::k_unTrackedDeviceIndexInvalid;

        vr::DriverPose_t pelvisPose{kDefaultPose};
        vr::DriverPose_t elbowLeftPose{kDefaultPose};
        vr::DriverPose_t elbowRightPose{kDefaultPose};
        vr::DriverPose_t kneeLeftPose{kDefaultPose};
        vr::DriverPose_t footLeftPose{kDefaultPose};
        vr::DriverPose_t kneeRightPose{kDefaultPose};
        vr::DriverPose_t footRightPose{kDefaultPose};

        vr::DriverPose_t handLeftPose{kDefaultPose};
        vr::DriverPose_t handRightPose{kDefaultPose};
        vr::DriverPose_t headPose{kDefaultPose};

        k4a::capture cap;
        k4abt::frame frame;

        try
        {
            k4abt::tracker tracker = k4abt::tracker::create(m_k4aCalibration, m_k4abtTrackerConfig);

            tracker.set_temporal_smoothing(m_k4abtSmoothingFactor);

            pelvisPose.deviceIsConnected = true;
            elbowLeftPose.deviceIsConnected = true;
            elbowRightPose.deviceIsConnected = true;
            kneeLeftPose.deviceIsConnected = true;
            footLeftPose.deviceIsConnected = true;
            kneeRightPose.deviceIsConnected = true;
            footRightPose.deviceIsConnected = true;

            if constexpr (kHandsAndHeadEnabled)
            {
                handLeftPose.deviceIsConnected = true;
                handRightPose.deviceIsConnected = true;
                headPose.deviceIsConnected = true;

                vr::VRServerDriverHost()->TrackedDevicePoseUpdated(handLeftId, handLeftPose, kDriverPoseSize);
                vr::VRServerDriverHost()->TrackedDevicePoseUpdated(handRightId, handRightPose, kDriverPoseSize);
                vr::VRServerDriverHost()->TrackedDevicePoseUpdated(headId, headPose, kDriverPoseSize);
            }

            vr::VRServerDriverHost()->TrackedDevicePoseUpdated(pelvisId, pelvisPose, kDriverPoseSize);
            vr::VRServerDriverHost()->TrackedDevicePoseUpdated(elbowLeftId, elbowLeftPose, kDriverPoseSize);
            vr::VRServerDriverHost()->TrackedDevicePoseUpdated(elbowRightId, elbowRightPose, kDriverPoseSize);
            vr::VRServerDriverHost()->TrackedDevicePoseUpdated(kneeLeftId, kneeLeftPose, kDriverPoseSize);
            vr::VRServerDriverHost()->TrackedDevicePoseUpdated(footLeftId, footLeftPose, kDriverPoseSize);
            vr::VRServerDriverHost()->TrackedDevicePoseUpdated(kneeRightId, kneeRightPose, kDriverPoseSize);
            vr::VRServerDriverHost()->TrackedDevicePoseUpdated(footRightId, footRightPose, kDriverPoseSize);

            while (m_trackingActive)
            {
                if (m_paused)
                {
                    std::this_thread::yield();
                    continue;
                }
                if (!m_k4aDevice.get_capture(&cap))
                    continue;
                else
                {

                    if (!tracker.enqueue_capture(cap, std::chrono::milliseconds(16)))
                    {
                        cap.reset();
                        continue;
                    }
                    else
                    {
                        frame = tracker.pop_result(std::chrono::milliseconds(16));
                        if (!frame)
                        {
                            cap.reset();
                            continue;
                        }
                        else
                        {
                            const uint32_t numBodies = frame.get_num_bodies();
                            if (numBodies != 0)
                            {
                                for (uint32_t i = 0; i < numBodies; i++)
                                {
                                    const auto skeleton = frame.get_body_skeleton(i);

                                    const auto depthImage = cap.get_depth_image();
                                    const auto depthImageDevTs = depthImage.get_device_timestamp();
                                    const auto depthImageSysTs = depthImage.get_system_timestamp();

                                    if (m_calibrationDataCount != m_calibrationDataMaxCount)
                                    {
                                        m_calibrationPoses[m_calibrationDataCount] = skeleton.joints[K4ABT_JOINT_HEAD];
                                        m_calibrationDataCount++;
                                    }

                                    if (m_calibrationUpdateReady)
                                    {
                                        UpdatePoseCalibration(pelvisPose);
                                        UpdatePoseCalibration(elbowLeftPose);
                                        UpdatePoseCalibration(elbowRightPose);
                                        UpdatePoseCalibration(kneeLeftPose);
                                        UpdatePoseCalibration(footLeftPose);
                                        UpdatePoseCalibration(kneeRightPose);
                                        UpdatePoseCalibration(footRightPose);

                                        if constexpr (kHandsAndHeadEnabled)
                                        {
                                            UpdatePoseCalibration(handLeftPose);
                                            UpdatePoseCalibration(handRightPose);
                                            UpdatePoseCalibration(headPose);
                                        }
                                        m_calibrationUpdateReady = false;
                                    }

                                    if (m_poseStreamActive && !m_headJointPoseReady)
                                    {
                                        m_headJointPose = skeleton.joints[K4ABT_JOINT_HEAD];
                                        m_headJointPoseReady = true;
                                    }

                                    UpdatePose(pelvisPose, skeleton.joints[K4ABT_JOINT_PELVIS]);
                                    UpdatePose(elbowLeftPose, skeleton.joints[K4ABT_JOINT_ELBOW_LEFT]);
                                    UpdatePose(elbowRightPose, skeleton.joints[K4ABT_JOINT_ELBOW_RIGHT]);
                                    UpdatePose(kneeLeftPose, skeleton.joints[K4ABT_JOINT_KNEE_LEFT]);
                                    UpdatePose(footLeftPose, skeleton.joints[K4ABT_JOINT_FOOT_LEFT]);
                                    UpdatePose(kneeRightPose, skeleton.joints[K4ABT_JOINT_KNEE_RIGHT]);
                                    UpdatePose(footRightPose, skeleton.joints[K4ABT_JOINT_FOOT_RIGHT]);

                                    if constexpr (kHandsAndHeadEnabled)
                                    {
                                        UpdatePose(handLeftPose, skeleton.joints[K4ABT_JOINT_HAND_LEFT]);
                                        UpdatePose(handRightPose, skeleton.joints[K4ABT_JOINT_HAND_RIGHT]);
                                        UpdatePose(headPose, skeleton.joints[K4ABT_JOINT_HEAD]);

                                        vr::VRServerDriverHost()->TrackedDevicePoseUpdated(handLeftId, handLeftPose,
                                                                                           kDriverPoseSize);
                                        vr::VRServerDriverHost()->TrackedDevicePoseUpdated(handRightId, handRightPose,
                                                                                           kDriverPoseSize);
                                        vr::VRServerDriverHost()->TrackedDevicePoseUpdated(headId, headPose,
                                                                                           kDriverPoseSize);
                                    }

                                    vr::VRServerDriverHost()->TrackedDevicePoseUpdated(pelvisId, pelvisPose,
                                                                                       kDriverPoseSize);
                                    vr::VRServerDriverHost()->TrackedDevicePoseUpdated(elbowLeftId, elbowLeftPose,
                                                                                       kDriverPoseSize);
                                    vr::VRServerDriverHost()->TrackedDevicePoseUpdated(elbowRightId, elbowRightPose,
                                                                                       kDriverPoseSize);
                                    vr::VRServerDriverHost()->TrackedDevicePoseUpdated(kneeLeftId, kneeLeftPose,
                                                                                       kDriverPoseSize);
                                    vr::VRServerDriverHost()->TrackedDevicePoseUpdated(footLeftId, footLeftPose,
                                                                                       kDriverPoseSize);
                                    vr::VRServerDriverHost()->TrackedDevicePoseUpdated(kneeRightId, kneeRightPose,
                                                                                       kDriverPoseSize);
                                    vr::VRServerDriverHost()->TrackedDevicePoseUpdated(footRightId, footRightPose,
                                                                                       kDriverPoseSize);
                                }
                            }
                            frame.reset();
                        }
                    }
                    cap.reset();
                }

            } // while(m_trackingActive)
            tracker.destroy();
        }
        catch (k4a::error &e)
        {
            DriverLog("K4A SDK: %s\n", e.what());
        }
    });
}

void TrackedJointProvider::StopTracking()
{
    if (m_trackingActive)
    {
        m_trackingActive = false;
        m_trackingThread.join();
    }
}

void TrackedJointProvider::RestartTracking()
{
    StopTracking();
    StartTracking();
}

} // namespace K4AOpenVR