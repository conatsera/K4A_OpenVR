#pragma once
#ifndef K4A_OPENVR_BONE_PROVIDER_H
#define K4A_OPENVR_BONE_PROVIDER_H

#include "k4a/k4a.h"
#include "k4abt.h"
#include "k4abttypes.h"

#include <openvr_driver.h>

#include "../extern/ctpl/ctpl_stl.h"
#include <thread>
#include <cmath>
#include "bone_filter.h"

typedef void(*DriverLog_t)(const char* pMsgFormat, ...);

typedef struct _joint_offset
{
	float w;
	float x;
	float y;
	float z;
} joint_offset_t;

typedef struct _calibration_data
{
	bool update;
	bool moreTrackers;
	// TODO: make this setting togglable for less powerful GPU setups. 
	// Right now camera is set to 15fps by default as seen in https://github.com/microsoft/Azure-Kinect-Sensor-SDK/issues/514
	bool lessCameraFPS;

	float x;
	float y;
	float z;
	float m_smoothing;
	float fps;

	joint_offset_t rotOffset;
} calibration_data_t;

#define	CALIBRATION_MEMSIZE sizeof(calibration_data_t)

inline vr::HmdQuaternion_t QuaternionProduct(vr::HmdQuaternion_t& quata, vr::HmdQuaternion_t& quatb)
{
	vr::HmdQuaternion_t quat;
	quat.w = ((quata.w * quatb.w) - (quata.x * quatb.x) - (quata.y * quatb.y) - (quata.z * quatb.z));
	quat.x = ((quata.w * quatb.x) + (quata.x * quatb.w) + (quata.y * quatb.z) - (quata.z * quatb.y));
	quat.y = ((quata.w * quatb.y) - (quata.x * quatb.z) + (quata.y * quatb.w) + (quata.z * quatb.x));
	quat.z = ((quata.w * quatb.z) + (quata.x * quatb.y) - (quata.y * quatb.x) + (quata.z * quatb.w));
	return quat;
}

inline vr::HmdQuaternion_t QuaternionProduct(vr::HmdQuaternion_t& quata, k4a_quaternion_t& quatb)
{
	vr::HmdQuaternion_t quat;
	quat.w = ((quata.w * quatb.wxyz.w) - (quata.x * quatb.wxyz.x) - (quata.y * quatb.wxyz.y) - (quata.z * quatb.wxyz.z));
	quat.x = ((quata.w * quatb.wxyz.x) + (quata.x * quatb.wxyz.w) + (quata.y * quatb.wxyz.z) - (quata.z * quatb.wxyz.y));
	quat.y = ((quata.w * quatb.wxyz.y) - (quata.x * quatb.wxyz.z) + (quata.y * quatb.wxyz.w) + (quata.z * quatb.wxyz.x));
	quat.z = ((quata.w * quatb.wxyz.z) + (quata.x * quatb.wxyz.y) - (quata.y * quatb.wxyz.x) + (quata.z * quatb.wxyz.w));
	return quat;
}

inline vr::HmdQuaternionf_t QuaternionProduct(k4a_quaternion_t& quata, k4a_quaternion_t& quatb)
{
	vr::HmdQuaternionf_t quat;
	quat.w = ((quata.wxyz.w * quatb.wxyz.w) - (quata.wxyz.x * quatb.wxyz.x) - (quata.wxyz.y * quatb.wxyz.y) - (quata.wxyz.z * quatb.wxyz.z));
	quat.x = ((quata.wxyz.w * quatb.wxyz.x) + (quata.wxyz.x * quatb.wxyz.w) + (quata.wxyz.y * quatb.wxyz.z) - (quata.wxyz.z * quatb.wxyz.y));
	quat.y = ((quata.wxyz.w * quatb.wxyz.y) - (quata.wxyz.x * quatb.wxyz.z) + (quata.wxyz.y * quatb.wxyz.w) + (quata.wxyz.z * quatb.wxyz.x));
	quat.z = ((quata.wxyz.w * quatb.wxyz.z) + (quata.wxyz.x * quatb.wxyz.y) - (quata.wxyz.y * quatb.wxyz.x) + (quata.wxyz.z * quatb.wxyz.w));
	return quat;
}

inline vr::HmdQuaternion_t QuaternionInverse(vr::HmdQuaternion_t& quat)
{
	vr::HmdQuaternion_t inverse_quat;
	inverse_quat.w = quat.w;
	inverse_quat.x = -(quat.x);
	inverse_quat.y = -(quat.y);
	inverse_quat.z = -(quat.z);
	return inverse_quat;
}

inline k4a_quaternion_t QuaternionInverse(k4a_quaternion_t& quat)
{
	k4a_quaternion_t inverse_quat;
	inverse_quat.wxyz.w = quat.wxyz.w;
	inverse_quat.wxyz.x = -(quat.wxyz.x);
	inverse_quat.wxyz.y = -(quat.wxyz.y);
	inverse_quat.wxyz.z = -(quat.wxyz.z);
	return inverse_quat;
}

typedef enum _K4ABoneProviderError
{
	BONE_PROVIDER_NO_ERROR,
	BONE_PROVIDER_OPEN_ERROR,
	BONE_PROVIDER_CAMERA_START_ERROR,
	BONE_PROVIDER_TRACKER_START_ERROR,
	BONE_PROVIDER_TRACKER_QUEUE_CAP_ERROR,
	BONE_PROVIDER_TRACKER_FRAME_POP_ERROR,
	BONE_PROVIDER_CALIB_ERROR,
	BONE_PROVIDER_MUTEX_ERROR
} K4ABoneProviderError;

class K4ABoneProvider
{
public:
	K4ABoneProvider(DriverLog_t driver_log);
	~K4ABoneProvider();

	K4ABoneProviderError Start();
	K4ABoneProviderError Configure(k4a_depth_mode_t new_depth_mode, float new_smoothing_rate);
	K4ABoneProviderError Stop();

	bool IsOnline() const {
		return m_online;
	};

	K4ABoneProviderError GetLastError() const
	{
		return m_error;
	};
	k4a_depth_mode_t GetDepthMode() const
	{
		return m_device_config.depth_mode;
	};
	float GetSmoothingRate() const
	{
		return m_smoothing_rate;
	};

	DriverLog_t m_driver_log;

	void setup_bone(uint32_t unObjectId, k4abt_joint_id_t bone);

private:
	std::thread* m_bone_thread = nullptr;

	k4a_device_t m_device = NULL;

	k4a_device_configuration_t m_device_config = K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;
	k4a_calibration_t m_calibration = { 0 };
	float m_smoothing_rate = 0.1F;

	K4ABoneProviderError m_error = BONE_PROVIDER_NO_ERROR;

	bool m_open = false;
	bool m_online = false;

protected:
	static void ProcessBones(K4ABoneProvider* context);

	k4abt_tracker_t m_tracker = NULL;

	vr::DriverPose_t m_hip_pose = { 0 };
	vr::DriverPose_t m_rleg_pose = { 0 };
	vr::DriverPose_t m_lleg_pose = { 0 };
	vr::DriverPose_t m_rknee_pose = { 0 };
	vr::DriverPose_t m_lknee_pose = { 0 };
	vr::DriverPose_t m_relbow_pose = { 0 };
	vr::DriverPose_t m_lelbow_pose = { 0 };
	vr::DriverPose_t m_chest_pose = { 0 };
	

	uint32_t m_hip_id = vr::k_unTrackedDeviceIndexInvalid;
	uint32_t m_rleg_id = vr::k_unTrackedDeviceIndexInvalid;
	uint32_t m_lleg_id = vr::k_unTrackedDeviceIndexInvalid;
	uint32_t m_rknee_id = vr::k_unTrackedDeviceIndexInvalid;
	uint32_t m_lknee_id = vr::k_unTrackedDeviceIndexInvalid;
	uint32_t m_relbow_id = vr::k_unTrackedDeviceIndexInvalid;
	uint32_t m_lelbow_id = vr::k_unTrackedDeviceIndexInvalid;
	uint32_t m_chest_id = vr::k_unTrackedDeviceIndexInvalid;
	

	bool m_calibrated = false;
};

#endif