#include <openvr_driver.h>
#include <thread>
#include <math.h>
#include "bone_provider.h"
#include "bone_filter.h"
#include <time.h>
#include <fstream>
#include <string>
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
			calibrationMem->update = false;

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
		m_relbow_pose.deviceIsConnected = false;
		m_lelbow_pose.deviceIsConnected = false;
		m_rknee_pose.deviceIsConnected = false;
		m_lknee_pose.deviceIsConnected = false;
		m_chest_pose.deviceIsConnected = false;

		m_hip_pose.poseIsValid = false;
		m_rleg_pose.poseIsValid = false;
		m_lleg_pose.poseIsValid = false;
		m_relbow_pose.poseIsValid = false;
		m_lelbow_pose.poseIsValid = false;
		m_rknee_pose.poseIsValid = false;
		m_lknee_pose.poseIsValid = false;
		m_chest_pose.poseIsValid = false;

		vr::VRServerDriverHost()->TrackedDevicePoseUpdated(m_hip_id, m_hip_pose, sizeof(vr::DriverPose_t));
		vr::VRServerDriverHost()->TrackedDevicePoseUpdated(m_rleg_id, m_rleg_pose, sizeof(vr::DriverPose_t));
		vr::VRServerDriverHost()->TrackedDevicePoseUpdated(m_lleg_id, m_lleg_pose, sizeof(vr::DriverPose_t));
		vr::VRServerDriverHost()->TrackedDevicePoseUpdated(m_relbow_id, m_relbow_pose, sizeof(vr::DriverPose_t));
		vr::VRServerDriverHost()->TrackedDevicePoseUpdated(m_lelbow_id, m_lelbow_pose, sizeof(vr::DriverPose_t));
		vr::VRServerDriverHost()->TrackedDevicePoseUpdated(m_rknee_id, m_rknee_pose, sizeof(vr::DriverPose_t));
		vr::VRServerDriverHost()->TrackedDevicePoseUpdated(m_lknee_id, m_lknee_pose, sizeof(vr::DriverPose_t));
		vr::VRServerDriverHost()->TrackedDevicePoseUpdated(m_chest_id, m_chest_pose, sizeof(vr::DriverPose_t));

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

void UpdateCalibration(vr::DriverPose_t *poses)
{
	// TODO:
	// if default values are changed, save changes to file
	// then
	// push file values onto variables

	float x = calibrationMem->x;
	float y = calibrationMem->y;
	float z = calibrationMem->z;

	float qw = calibrationMem->rotOffset.w;
	float qx = calibrationMem->rotOffset.x;
	float qy = calibrationMem->rotOffset.y;
	float qz = calibrationMem->rotOffset.z;

	// updates calibration for every bone pose
	for (int i = 0; i < 8; i++) {
		poses[i].vecWorldFromDriverTranslation[0] = x;
		poses[i].vecWorldFromDriverTranslation[1] = y;
		poses[i].vecWorldFromDriverTranslation[2] = z;

		poses[i].qWorldFromDriverRotation.w = qw;
		poses[i].qWorldFromDriverRotation.x = qx;
		poses[i].qWorldFromDriverRotation.y = qy;
		poses[i].qWorldFromDriverRotation.z = qz;
	}

	calibrationMem->update = false;
}

void K4ABoneProvider::ProcessBones(K4ABoneProvider* context)
{

	// Wait for every bone to be activated before attempting to populate pose data
	while (context->m_hip_id == vr::k_unTrackedDeviceIndexInvalid || context->m_rleg_id == vr::k_unTrackedDeviceIndexInvalid || context->m_lleg_id == vr::k_unTrackedDeviceIndexInvalid || context->m_relbow_id == vr::k_unTrackedDeviceIndexInvalid || context->m_lelbow_id == vr::k_unTrackedDeviceIndexInvalid
		|| context->m_rknee_id == vr::k_unTrackedDeviceIndexInvalid || context->m_lknee_id == vr::k_unTrackedDeviceIndexInvalid || context->m_chest_id == vr::k_unTrackedDeviceIndexInvalid)
		std::this_thread::sleep_for(std::chrono::milliseconds(33));

	// Bring thread relevent data into the thread stack

	k4a_device_t device = context->m_device;
	k4abt_tracker_t tracker;

	uint32_t ids[8];
	ids[0] = context->m_hip_id;
	ids[1] = context->m_lleg_id;
	ids[2] = context->m_rleg_id;
	ids[3] = context->m_chest_id;
	ids[4] = context->m_relbow_id;
	ids[5] = context->m_lelbow_id;
	ids[6] = context->m_rknee_id;
	ids[7] = context->m_lknee_id;


	k4a_capture_t capture = nullptr;
	k4abt_frame_t body_frame = nullptr;

	if (k4abt_tracker_create(&context->m_calibration, ::K4ABT_TRACKER_CONFIG_DEFAULT, &tracker) == K4A_RESULT_SUCCEEDED)
	{
		k4abt_tracker_set_temporal_smoothing(tracker, 0);

		if (!context->IsOnline())
		{
			context->m_online = true;

			context->m_hip_pose.deviceIsConnected = true;
			context->m_rleg_pose.deviceIsConnected = true;
			context->m_lleg_pose.deviceIsConnected = true;
			context->m_chest_pose.deviceIsConnected = true;
			context->m_relbow_pose.deviceIsConnected = true;
			context->m_lelbow_pose.deviceIsConnected = true;
			context->m_rknee_pose.deviceIsConnected = true;
			context->m_lknee_pose.deviceIsConnected = true;

			vr::VRServerDriverHost()->TrackedDevicePoseUpdated(ids[0], context->m_hip_pose, sizeof(vr::DriverPose_t));
			vr::VRServerDriverHost()->TrackedDevicePoseUpdated(ids[1], context->m_lleg_pose, sizeof(vr::DriverPose_t));
			vr::VRServerDriverHost()->TrackedDevicePoseUpdated(ids[2], context->m_rleg_pose, sizeof(vr::DriverPose_t));
			vr::VRServerDriverHost()->TrackedDevicePoseUpdated(ids[3], context->m_chest_pose, sizeof(vr::DriverPose_t));
			vr::VRServerDriverHost()->TrackedDevicePoseUpdated(ids[4], context->m_relbow_pose, sizeof(vr::DriverPose_t));
			vr::VRServerDriverHost()->TrackedDevicePoseUpdated(ids[5], context->m_lelbow_pose, sizeof(vr::DriverPose_t));
			vr::VRServerDriverHost()->TrackedDevicePoseUpdated(ids[6], context->m_rknee_pose, sizeof(vr::DriverPose_t));
			vr::VRServerDriverHost()->TrackedDevicePoseUpdated(ids[7], context->m_lknee_pose, sizeof(vr::DriverPose_t));
		}

		uint64_t last_timestamp = 0;

		// recreate stack copies from the calibrated baseline pose
		vr::DriverPose_t poses[] = { context->m_hip_pose, context->m_lleg_pose, context->m_rleg_pose, context->m_rleg_pose, 
			context->m_chest_pose, context->m_relbow_pose, context->m_lelbow_pose, context->m_rknee_pose, context->m_lknee_pose};

		// create array of jointIDs for array access
		k4abt_joint_id_t jointIDs[] = { K4ABT_JOINT_PELVIS, K4ABT_JOINT_FOOT_LEFT, K4ABT_JOINT_FOOT_RIGHT, K4ABT_JOINT_SPINE_CHEST, 
		K4ABT_JOINT_ELBOW_RIGHT, K4ABT_JOINT_ELBOW_LEFT, K4ABT_JOINT_KNEE_RIGHT, K4ABT_JOINT_KNEE_LEFT};
		


		
		float smoothing = 0.0f;
		bool updateData = false;
		// create bone filter for each bone
		bone_filter filters[8];
		// initialize time variable
		clock_t lastTime;
		lastTime = clock();

		while (context->m_online)
		{
			if (k4a_device_get_capture(device, &capture, K4A_WAIT_INFINITE) != K4A_WAIT_RESULT_SUCCEEDED)
			{
				updateData = true;
				continue;
			}
			else
			{
				if (k4abt_tracker_enqueue_capture(tracker, capture, 0) != K4A_WAIT_RESULT_SUCCEEDED)
				{
					k4a_capture_release(capture);
					updateData = true;
					continue;
				}
				else
				{
					if (k4abt_tracker_pop_result(tracker, &body_frame, 0) != K4A_WAIT_RESULT_SUCCEEDED)
					{
						k4a_capture_release(capture);
						updateData = true;
						continue;
					}
					else
					{
						if (int num_bodies = k4abt_frame_get_num_bodies(body_frame) != 0)
						{
							if (calibrationMem->update)
								UpdateCalibration(poses);

							for (int i = 0; i < num_bodies; i++)
							{
								k4abt_skeleton_t skeleton;
								if (k4abt_frame_get_body_skeleton(body_frame, i, &skeleton) != K4A_RESULT_SUCCEEDED)
								{
									for (int i = 0; i < 8; i++) {
										poses[i].poseIsValid = false;
										vr::VRServerDriverHost()->TrackedDevicePoseUpdated(ids[i], poses[i], sizeof(vr::DriverPose_t));
									}
								}
								else
								{
									clock_t thisTime = clock();
									float timePassed = float(thisTime - lastTime) / CLOCKS_PER_SEC;

									// lambda update that updates a bone's position and rotation
									auto updateBone = [](bone_filter& boneFilter, k4abt_joint_t bone, vr::DriverPose_t& bone_pose, float timePassed) {
										k4abt_joint_t bonePrediction = boneFilter.getNextPos(bone);
										float temp;
										bone_pose.poseIsValid = true;
										bone_pose.qRotation.w = bone.orientation.wxyz.w;
										bone_pose.qRotation.x = bone.orientation.wxyz.z;
										bone_pose.qRotation.y = bone.orientation.wxyz.x;
										bone_pose.qRotation.z = bone.orientation.wxyz.y;
										temp = bone_pose.vecPosition[0];
										bone_pose.vecPosition[0] = (0.3 * (bone_pose.vecPosition[0] + (bone_pose.vecVelocity[0] * timePassed))) + (0.7 * (bonePrediction.position.xyz.z / 1000));
										bone_pose.vecVelocity[0] = (bone_pose.vecPosition[0] - temp) / timePassed;
										temp = bone_pose.vecPosition[1];
										bone_pose.vecPosition[1] = (0.3 * (bone_pose.vecPosition[1] + (bone_pose.vecVelocity[1] * timePassed))) + (0.7 * (bonePrediction.position.xyz.x / 1000));
										bone_pose.vecVelocity[1] = (bone_pose.vecPosition[1] - temp) / timePassed;
										temp = bone_pose.vecPosition[2];
										bone_pose.vecPosition[2] = (0.3 * (bone_pose.vecPosition[2] + (bone_pose.vecVelocity[2] * timePassed))) + (0.7 * (bonePrediction.position.xyz.y / 1000));
										bone_pose.vecVelocity[2] = (bone_pose.vecPosition[2] - temp) / timePassed;
										bone_pose.poseTimeOffset = timePassed;
									};

									if (calibrationMem->moreTrackers) {
										#pragma omp parallel for
										for (int i = 0; i < 8; i++) {
											updateBone(std::ref(filters[i]), skeleton.joints[jointIDs[i]], std::ref(poses[i]), timePassed);
											vr::VRServerDriverHost()->TrackedDevicePoseUpdated(ids[i], poses[i], sizeof(vr::DriverPose_t));
										}
									}
									else {
										#pragma omp parallel for
										for (int i = 0; i < 3; i++) {
											updateBone(std::ref(filters[i]), skeleton.joints[jointIDs[i]], std::ref(poses[i]), timePassed);
											vr::VRServerDriverHost()->TrackedDevicePoseUpdated(ids[i], poses[i], sizeof(vr::DriverPose_t));
										}
									}

									lastTime = clock();
									calibrationMem->fps = 1 / timePassed;
								}
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

	bone_pose.poseTimeOffset = 0.11F;

	bone_pose.qDriverFromHeadRotation.w = 1.F;
	bone_pose.qDriverFromHeadRotation.x = 0.F;
	bone_pose.qDriverFromHeadRotation.y = 0.F;
	bone_pose.qDriverFromHeadRotation.z = 0.F;

	bone_pose.qWorldFromDriverRotation.w = std::cos(std::acos(-1) / 4);
	bone_pose.qWorldFromDriverRotation.x = std::sin(std::acos(-1) / 4);
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
	else if (bone == K4ABT_JOINT_FOOT_LEFT)
	{
		m_lleg_pose = bone_pose;
		m_lleg_id = unObjectId;
	}
	else if (bone == K4ABT_JOINT_SPINE_CHEST)
	{
		m_chest_pose = bone_pose;
		m_chest_id = unObjectId;
	}
	else if (bone == K4ABT_JOINT_ELBOW_RIGHT)
	{
		m_relbow_pose = bone_pose;
		m_relbow_id = unObjectId;
	}
	else if (bone == K4ABT_JOINT_ELBOW_LEFT)
	{
		m_lelbow_pose = bone_pose;
		m_lelbow_id = unObjectId;
	}
	else if (bone == K4ABT_JOINT_KNEE_RIGHT)
	{
		m_rknee_pose = bone_pose;
		m_rknee_id = unObjectId;
	}
	else
	{
		m_lknee_pose = bone_pose;
		m_lknee_id = unObjectId;
	}

}



