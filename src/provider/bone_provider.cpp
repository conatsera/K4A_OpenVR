#include <openvr_driver.h>

#include "bone_provider.h"

#include <windows.h>

static TCHAR calibrationMemName[] = TEXT("BoneCalibrationMemmap");

static HANDLE calibrationMemHandle = INVALID_HANDLE_VALUE;
static calibration_data_t* calibrationMem = NULL;

static void k4a_log_cb(void* context,
	k4a_log_level_t level,
	const char* file,
	const int line,
	const char* message)
{
	((K4ABoneProvider*)context)->m_driver_log("%d : %s:%d - %s\n", level, file, line, message);
}

K4ABoneProvider::K4ABoneProvider(DriverLog_t driver_log)
{
	m_driver_log = driver_log;

	calibrationMemHandle = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		CALIBRATION_MEMSIZE,
		calibrationMemName
	);

	if (calibrationMemHandle == nullptr)
		driver_log("Could not open file mapping object %d\n", GetLastError());
	else
	{
		calibrationMem = reinterpret_cast<calibration_data_t*>((LPTSTR)MapViewOfFile(calibrationMemHandle,
			FILE_MAP_ALL_ACCESS,
			0,
			0,
			CALIBRATION_MEMSIZE));

		if (calibrationMem == nullptr)
		{
			driver_log("Could not map view of file %d\n", GetLastError());

			CloseHandle(calibrationMemHandle);
		}
		else
		{
			calibrationMem->x = 0.F;
			calibrationMem->y = 0.F;
			calibrationMem->z = 0.F;

			calibrationMem->rotOffset.w = 1.F;
			calibrationMem->rotOffset.x = 0.F;
			calibrationMem->rotOffset.y = 0.F;
			calibrationMem->rotOffset.z = 0.F;

			k4a_set_debug_message_handler(k4a_log_cb, this, k4a_log_level_t::K4A_LOG_LEVEL_ERROR);

			if (k4a_device_open(K4A_DEVICE_DEFAULT, &m_device) != K4A_RESULT_SUCCEEDED)
			{
				m_driver_log("Open K4A device failed\n");
				m_error = BONE_PROVIDER_OPEN_ERROR;
			}
			else
				m_open = true;
		}
	}
}

K4ABoneProvider::~K4ABoneProvider()
{
	k4a_device_close(m_device);
}

K4ABoneProviderError K4ABoneProvider::Configure(k4a_depth_mode_t new_depth_mode, float new_smoothing_rate)
{
	m_device_config.depth_mode = new_depth_mode;
	m_smoothing_rate = new_smoothing_rate;

	if (m_open)
	{
		if (K4A_RESULT_SUCCEEDED != k4a_device_get_calibration(m_device, m_device_config.depth_mode, K4A_COLOR_RESOLUTION_OFF, &m_calibration))
		{
			m_driver_log("Get depth camera calibration failed!\n");
			return BONE_PROVIDER_CALIB_ERROR;
		}
	}

	return BONE_PROVIDER_OPEN_ERROR;
}

K4ABoneProviderError K4ABoneProvider::Start()
{
	if (m_open)
	{

		if (k4a_device_start_cameras(m_device, &m_device_config) != K4A_RESULT_SUCCEEDED)
		{
			m_driver_log("Start camera failed\n");
			m_error = BONE_PROVIDER_CAMERA_START_ERROR;
			return m_error;
		}

		m_bone_thread = new std::thread(ProcessBones, this);
	}
	return BONE_PROVIDER_OPEN_ERROR;
}

K4ABoneProviderError K4ABoneProvider::Stop()
{
	if (m_open && m_online)
	{
		m_online = false;

		m_bone_thread->join();

		m_hip_pose.deviceIsConnected = false;
		m_rleg_pose.deviceIsConnected = false;
		m_lleg_pose.deviceIsConnected = false;

		m_hip_pose.poseIsValid = false;
		m_rleg_pose.poseIsValid = false;
		m_lleg_pose.poseIsValid = false;

		vr::VRServerDriverHost()->TrackedDevicePoseUpdated(m_hip_id, m_hip_pose, sizeof(vr::DriverPose_t));
		vr::VRServerDriverHost()->TrackedDevicePoseUpdated(m_rleg_id, m_rleg_pose, sizeof(vr::DriverPose_t));
		vr::VRServerDriverHost()->TrackedDevicePoseUpdated(m_lleg_id, m_lleg_pose, sizeof(vr::DriverPose_t));

		k4a_device_stop_cameras(m_device);

		m_error = BONE_PROVIDER_NO_ERROR;

		m_driver_log("Device is now offline for reconfiguration\n");
	}
	else
	{
		m_driver_log("Device is already offline\n");
	}

	return m_error;
}

void K4ABoneProvider::ProcessBones(K4ABoneProvider* context)
{
	// Wait for every bone to be activated before attempting to populate pose data
	while (context->m_hip_id == vr::k_unTrackedDeviceIndexInvalid || context->m_rleg_id == vr::k_unTrackedDeviceIndexInvalid || context->m_lleg_id == vr::k_unTrackedDeviceIndexInvalid)
		std::this_thread::sleep_for(std::chrono::milliseconds(33));

	// Bring thread relevent data into the thread stack

	k4a_device_t device = context->m_device;
	k4abt_tracker_t tracker;

	uint32_t hip_id = context->m_hip_id;
	uint32_t rleg_id = context->m_rleg_id;
	uint32_t lleg_id = context->m_lleg_id;

	k4a_capture_t capture = nullptr;
	k4abt_frame_t body_frame = nullptr;

	if (k4abt_tracker_create(&context->m_calibration, ::K4ABT_TRACKER_CONFIG_DEFAULT, &tracker) == K4A_RESULT_SUCCEEDED)
	{
		k4abt_tracker_set_temporal_smoothing(tracker, 0.075F);

		if (!context->IsOnline())
		{
			context->m_online = true;

			context->m_hip_pose.deviceIsConnected = true;
			context->m_rleg_pose.deviceIsConnected = true;
			context->m_lleg_pose.deviceIsConnected = true;

			vr::VRServerDriverHost()->TrackedDevicePoseUpdated(hip_id, context->m_hip_pose, sizeof(vr::DriverPose_t));
			vr::VRServerDriverHost()->TrackedDevicePoseUpdated(rleg_id, context->m_rleg_pose, sizeof(vr::DriverPose_t));
			vr::VRServerDriverHost()->TrackedDevicePoseUpdated(lleg_id, context->m_lleg_pose, sizeof(vr::DriverPose_t));
		}

		uint64_t last_timestamp = 0;

		// recreate stack copies from the calibrated baseline pose
		vr::DriverPose_t hip_pose = context->m_hip_pose;
		vr::DriverPose_t rleg_pose = context->m_rleg_pose;
		vr::DriverPose_t lleg_pose = context->m_lleg_pose;

		while (context->m_online)
		{
			if (k4a_device_get_capture(device, &capture, K4A_WAIT_INFINITE) != K4A_WAIT_RESULT_SUCCEEDED)
				continue;
			else
			{
				if (k4abt_tracker_enqueue_capture(tracker, capture, 16) != K4A_WAIT_RESULT_SUCCEEDED)
				{
					k4a_capture_release(capture);
					continue;
				}
				else
				{
					if (k4abt_tracker_pop_result(tracker, &body_frame, 16) != K4A_WAIT_RESULT_SUCCEEDED)
					{
						k4a_capture_release(capture);
						continue;
					}
					else
					{
						if (k4abt_frame_get_num_bodies(body_frame) != 0)
						{
							if (calibrationMem->update)
							{
								float x = calibrationMem->x;
								float y = calibrationMem->y;
								float z = calibrationMem->z;

								float qw = calibrationMem->rotOffset.w;
								float qx = calibrationMem->rotOffset.x;
								float qy = calibrationMem->rotOffset.y;
								float qz = calibrationMem->rotOffset.z;

								hip_pose.vecWorldFromDriverTranslation[0] = x;
								hip_pose.vecWorldFromDriverTranslation[1] = y;
								hip_pose.vecWorldFromDriverTranslation[2] = z;

								rleg_pose.vecWorldFromDriverTranslation[0] = x;
								rleg_pose.vecWorldFromDriverTranslation[1] = y;
								rleg_pose.vecWorldFromDriverTranslation[2] = z;

								lleg_pose.vecWorldFromDriverTranslation[0] = x;
								lleg_pose.vecWorldFromDriverTranslation[1] = y;
								lleg_pose.vecWorldFromDriverTranslation[2] = z;

								hip_pose.qWorldFromDriverRotation.w = qw;
								hip_pose.qWorldFromDriverRotation.x = qx;
								hip_pose.qWorldFromDriverRotation.y = qy;
								hip_pose.qWorldFromDriverRotation.z = qz;
								                                     
								rleg_pose.qWorldFromDriverRotation.w = qw;
								rleg_pose.qWorldFromDriverRotation.x = qx;
								rleg_pose.qWorldFromDriverRotation.y = qy;
								rleg_pose.qWorldFromDriverRotation.z = qz;

								lleg_pose.qWorldFromDriverRotation.w = qw;
								lleg_pose.qWorldFromDriverRotation.x = qx;
								lleg_pose.qWorldFromDriverRotation.y = qy;
								lleg_pose.qWorldFromDriverRotation.z = qz;
							}
							
							k4abt_skeleton_t skeleton;
							if (k4abt_frame_get_body_skeleton(body_frame, 0, &skeleton) != K4A_RESULT_SUCCEEDED)
							{
								hip_pose.poseIsValid = false;
								rleg_pose.poseIsValid = false;
								lleg_pose.poseIsValid = false;

								vr::VRServerDriverHost()->TrackedDevicePoseUpdated(hip_id, hip_pose, sizeof(vr::DriverPose_t));
								vr::VRServerDriverHost()->TrackedDevicePoseUpdated(rleg_id, rleg_pose, sizeof(vr::DriverPose_t));
								vr::VRServerDriverHost()->TrackedDevicePoseUpdated(lleg_id, lleg_pose, sizeof(vr::DriverPose_t));
							}
							else
							{
								hip_pose.poseIsValid = true;

								hip_pose.qRotation.w = skeleton.joints[K4ABT_JOINT_PELVIS].orientation.wxyz.w;
								hip_pose.qRotation.x = skeleton.joints[K4ABT_JOINT_PELVIS].orientation.wxyz.z;
								hip_pose.qRotation.y = skeleton.joints[K4ABT_JOINT_PELVIS].orientation.wxyz.x;
								hip_pose.qRotation.z = skeleton.joints[K4ABT_JOINT_PELVIS].orientation.wxyz.y;

								hip_pose.vecPosition[0] = skeleton.joints[K4ABT_JOINT_PELVIS].position.xyz.z / 1000;
								hip_pose.vecPosition[1] = skeleton.joints[K4ABT_JOINT_PELVIS].position.xyz.x / 1000;
								hip_pose.vecPosition[2] = skeleton.joints[K4ABT_JOINT_PELVIS].position.xyz.y / 1000;

								rleg_pose.poseIsValid = true;

								rleg_pose.qRotation.w = skeleton.joints[K4ABT_JOINT_ANKLE_RIGHT].orientation.wxyz.w;
								rleg_pose.qRotation.x = skeleton.joints[K4ABT_JOINT_ANKLE_RIGHT].orientation.wxyz.z;
								rleg_pose.qRotation.y = skeleton.joints[K4ABT_JOINT_ANKLE_RIGHT].orientation.wxyz.x;
								rleg_pose.qRotation.z = skeleton.joints[K4ABT_JOINT_ANKLE_RIGHT].orientation.wxyz.y;

								rleg_pose.vecPosition[0] = skeleton.joints[K4ABT_JOINT_ANKLE_RIGHT].position.xyz.z / 1000;
								rleg_pose.vecPosition[1] = skeleton.joints[K4ABT_JOINT_ANKLE_RIGHT].position.xyz.x / 1000;
								rleg_pose.vecPosition[2] = skeleton.joints[K4ABT_JOINT_ANKLE_RIGHT].position.xyz.y / 1000;

								lleg_pose.poseIsValid = true;

								lleg_pose.qRotation.w = skeleton.joints[K4ABT_JOINT_ANKLE_LEFT].orientation.wxyz.w;
								lleg_pose.qRotation.x = skeleton.joints[K4ABT_JOINT_ANKLE_LEFT].orientation.wxyz.z;
								lleg_pose.qRotation.y = skeleton.joints[K4ABT_JOINT_ANKLE_LEFT].orientation.wxyz.x;
								lleg_pose.qRotation.z = skeleton.joints[K4ABT_JOINT_ANKLE_LEFT].orientation.wxyz.y;

								lleg_pose.vecPosition[0] = skeleton.joints[K4ABT_JOINT_ANKLE_LEFT].position.xyz.z / 1000;
								lleg_pose.vecPosition[1] = skeleton.joints[K4ABT_JOINT_ANKLE_LEFT].position.xyz.x / 1000;
								lleg_pose.vecPosition[2] = skeleton.joints[K4ABT_JOINT_ANKLE_LEFT].position.xyz.y / 1000;

								vr::VRServerDriverHost()->TrackedDevicePoseUpdated(hip_id, hip_pose, sizeof(vr::DriverPose_t));
								vr::VRServerDriverHost()->TrackedDevicePoseUpdated(rleg_id, rleg_pose, sizeof(vr::DriverPose_t));
								vr::VRServerDriverHost()->TrackedDevicePoseUpdated(lleg_id, lleg_pose, sizeof(vr::DriverPose_t));
							}
						}
						k4abt_frame_release(body_frame);
					}
				}
				k4a_capture_release(capture);
			}
		}

		k4abt_tracker_destroy(tracker);
	}
}

void K4ABoneProvider::setup_bone(uint32_t unObjectId, k4abt_joint_id_t bone)
{
	vr::DriverPose_t bone_pose = { 0 };

	bone_pose.poseTimeOffset = 0.F;

	bone_pose.qDriverFromHeadRotation.w = 1.F;
	bone_pose.qDriverFromHeadRotation.x = 0.F;
	bone_pose.qDriverFromHeadRotation.y = 0.F;
	bone_pose.qDriverFromHeadRotation.z = 0.F;

	bone_pose.qWorldFromDriverRotation.w = 1.F;
	bone_pose.qWorldFromDriverRotation.x = 0.F;
	bone_pose.qWorldFromDriverRotation.y = 0.F;
	bone_pose.qWorldFromDriverRotation.z = 0.F;

	bone_pose.vecWorldFromDriverTranslation[0] = 0.F;
	bone_pose.vecWorldFromDriverTranslation[1] = 0.F;
	bone_pose.vecWorldFromDriverTranslation[2] = 0.F;

	bone_pose.vecDriverFromHeadTranslation[0] = 0.F;
	bone_pose.vecDriverFromHeadTranslation[1] = 0.F;
	bone_pose.vecDriverFromHeadTranslation[2] = 0.F;

	bone_pose.vecVelocity[0] = 0.F;
	bone_pose.vecVelocity[1] = 0.F;
	bone_pose.vecVelocity[2] = 0.F;

	bone_pose.vecAcceleration[0] = 0.F;
	bone_pose.vecAcceleration[1] = 0.F;
	bone_pose.vecAcceleration[2] = 0.F;

	bone_pose.vecAngularVelocity[0] = 0.0;
	bone_pose.vecAngularVelocity[1] = 0.0;
	bone_pose.vecAngularVelocity[2] = 0.0;

	bone_pose.vecAngularAcceleration[0] = 0.0;
	bone_pose.vecAngularAcceleration[1] = 0.0;
	bone_pose.vecAngularAcceleration[2] = 0.0;

	bone_pose.willDriftInYaw = false;
	bone_pose.shouldApplyHeadModel = false;

	if (bone == K4ABT_JOINT_PELVIS)
	{
		m_hip_pose = bone_pose;
		m_hip_id = unObjectId;
	}
	else if (bone == K4ABT_JOINT_FOOT_RIGHT)
	{
		m_rleg_pose = bone_pose;
		m_rleg_id = unObjectId;
	}
	else
	{
		m_lleg_pose = bone_pose;
		m_lleg_id = unObjectId;
	}
}