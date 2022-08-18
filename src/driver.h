#ifndef K4A_OPENVR_DRIVER_H
#define K4A_OPENVR_DRIVER_H

#include <array>
#include <memory>
#include <thread>

#include <k4abt.hpp>
#include <openvr_driver.h>

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include "driverlog.h"

#include "calibration.grpc.pb.h"

namespace K4AOpenVR
{

constexpr const bool kHandsAndHeadEnabled = true;

class WatchdogProvider : public vr::IVRWatchdogProvider
{
  public:
    virtual vr::EVRInitError Init(vr::IVRDriverContext *pDriverContext);
    virtual void Cleanup();

  private:
    bool m_exiting = false;
    std::thread m_pWatchdogThread;
};

class TrackedJointDriver : public vr::ITrackedDeviceServerDriver
{
  public:
    TrackedJointDriver(k4abt_joint_id_t jointId);
    virtual ~TrackedJointDriver()
    {
    }

    virtual vr::EVRInitError Activate(vr::TrackedDeviceIndex_t unObjectId) override;
    virtual void Deactivate() override;

    virtual void EnterStandby() override;

    virtual void *GetComponent(const char *pchComponentNameAndVersion) override;

    virtual void DebugRequest(const char *pchRequest, char *pchResponseBuffer, uint32_t unResponseBufferSize) override;

    const std::string GetSerialNumber() const
    {
        return m_sModelName;
    }

    virtual vr::DriverPose_t GetPose()
    {
        return vr::DriverPose_t{0};
    }

    bool IsActivated() const
    {
        return m_unObjectId != vr::k_unTrackedDeviceIndexInvalid;
    }

    const vr::TrackedDeviceIndex_t GetIndex() const
    {
        return m_unObjectId;
    }

  private:
    vr::TrackedDeviceIndex_t m_unObjectId = vr::k_unTrackedDeviceIndexInvalid;
    vr::PropertyContainerHandle_t m_ulPropertyContainer = vr::k_ulInvalidPropertyContainer;

    k4abt_joint_id_t m_jointId;
    std::string m_sModelName;
};

class TrackedJointProvider final : public vr::IServerTrackedDeviceProvider,
                                   public calibration::Settings::Service,
                                   public calibration::Calibration::Service
{
  public:
    virtual vr::EVRInitError Init(vr::IVRDriverContext *pDriverContext) override;
    virtual void Cleanup() override;

    virtual const char *const *GetInterfaceVersions() override
    {
        return vr::k_InterfaceVersions;
    }

    virtual void RunFrame() override{};
    virtual bool ShouldBlockStandbyMode() override
    {
        return false;
    }
    virtual void EnterStandby() override
    {
    }
    virtual void LeaveStandby() override
    {
    }

    grpc::Status Get(grpc::ServerContext *context, const calibration::SettingsGetRequest *request,
                     calibration::SettingsUpdateRequest *reply) override;

    grpc::Status Update(grpc::ServerContext *context, const calibration::SettingsUpdateRequest *request,
                        calibration::SettingsReply *reply) override;

    grpc::Status PauseResume(grpc::ServerContext *context, const calibration::PauseResumeRequest *request,
                             calibration::PauseResumeReply *reply) override;


    grpc::Status GetCalibrationData(grpc::ServerContext *context, const calibration::CalibrationDataRequest *request,
                                    calibration::CalibrationDataReply *reply) override;

    grpc::Status UpdateCalibration(grpc::ServerContext *context, const calibration::UpdateCalibrationRequest *request,
                                   calibration::UpdateCalibrationReply *reply) override;


    grpc::Status GetPoseStream(grpc::ServerContext *context, const calibration::CalibrationDataRequest *request,
                               grpc::ServerWriter<::calibration::CalibrationData> *writer) override;

    grpc::Status StopPoseStream(grpc::ServerContext *context, const calibration::CalibrationDataRequest *request,
                                calibration::UpdateCalibrationReply *reply) override;

  private:
    void ConfigureK4A();

    void StartTracking();
    void StopTracking();

    void RestartTracking();

    bool DriversActivated() const;

    inline void UpdatePoseCalibration(vr::DriverPose_t &driverPose);

  private:
    TrackedJointDriver *m_pelvisJoint;
    TrackedJointDriver *m_elbowLeftJoint;
    TrackedJointDriver *m_handLeftJoint;
    TrackedJointDriver *m_elbowRightJoint;
    TrackedJointDriver *m_handRightJoint;
    TrackedJointDriver *m_kneeLeftJoint;
    TrackedJointDriver *m_footLeftJoint;
    TrackedJointDriver *m_kneeRightJoint;
    TrackedJointDriver *m_footRightJoint;
    TrackedJointDriver *m_headJoint;

    k4a::device m_k4aDevice;
    k4a_device_configuration_t m_k4aConfig = K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;
    k4a::calibration m_k4aCalibration{0};

    bool m_k4abtLiteMode = false;
    k4abt_tracker_configuration_t m_k4abtTrackerConfig = K4ABT_TRACKER_CONFIG_DEFAULT;
    float m_k4abtSmoothingFactor = 0.1f;

    uint32_t m_calibrationDataCount = 30;
    uint32_t m_calibrationDataMaxCount = 30;
    std::array<k4abt_joint_t, 30> m_calibrationPoses;

    vr::HmdQuaternion_t m_qWorldFromDriverRotation{1.0, 0.0, 0.0, 0.0};
    double m_vecWorldFromDriverTranslation[3]{0.0, 0.0, 0.0};
    bool m_calibrationUpdateReady = false;

    bool m_poseStreamActive = false;
    k4abt_joint_t m_headJointPose{k4a_float3_t{0.f, 0.f, 0.f}, k4a_quaternion_t{1.f, 0.f, 0.f, 0.f},
                                  k4abt_joint_confidence_level_t::K4ABT_JOINT_CONFIDENCE_NONE};
    bool m_headJointPoseReady = false;

    bool m_trackingActive = false;
    bool m_paused = false;
    std::thread m_trackingThread;

    bool m_exiting = false;
    std::unique_ptr<grpc::Server> m_calibratorServer;
    std::thread m_calibratorThread;
};

} // namespace K4AOpenVR

#endif