// K4A OpenVR.cpp : Defines the entry point for the application.
//

#include "k4a_driver.h"

#include <future>

bool g_bExiting = false;

vr::EVRInitError K4AWatchdogDriver::Init(vr::IVRDriverContext* pDriverContext) {
	VR_INIT_WATCHDOG_DRIVER_CONTEXT(pDriverContext);
	InitDriverLog(vr::VRDriverLog());

	g_bExiting = false;

	return vr::VRInitError_None;
}

void K4AWatchdogDriver::Cleanup() {
	g_bExiting = true;

	CleanupDriverLog();
}

K4ATrackerDriver::K4ATrackerDriver(K4ATrackedBone bone, K4ABoneProvider* bone_provider) : m_bone_provider(bone_provider) {
	m_unObjectId = vr::k_unTrackedDeviceIndexInvalid;
	m_ulPropertyContainer = vr::k_ulInvalidPropertyContainer;

	m_bone = bone;
	m_pose = { 0 };

	if (bone == K4ATrackedBoneHip) {
		m_sSerialNumber = "Hip";
		m_sModelNumber = "K4A_DK_BNODE_HIP";
	}
	else if (bone == K4ATrackedBoneRightFoot) {
		m_sSerialNumber = "RFoot";
		m_sModelNumber = "K4A_DK_BNODE_RFOOT";
	}
	else if (bone == K4ATrackedBoneLeftFoot) {
		m_sSerialNumber = "LFoot";
		m_sModelNumber = "K4A_DK_BNODE_LFOOT";
	}
	else if (bone == K4ATrackedBoneChest) {
		m_sSerialNumber = "Chest";
		m_sModelNumber = "K4A_DK_BNODE_CHEST";
	}
	else if (bone == K4ATrackedBoneRightElbow) {
		m_sSerialNumber = "RElbow";
		m_sModelNumber = "K4A_DK_BNODE_RELBOW";
	}
	else if (bone == K4ATrackedBoneLeftElbow) {
		m_sSerialNumber = "LElbow";
		m_sModelNumber = "K4A_DK_BNODE_LELBOW";
	}
	else if (bone == k4ATrackedBoneRightKnee) {
		m_sSerialNumber = "RKnee";
		m_sModelNumber = "K4A_DK_BNODE_RKNEE";
	}
	else if (bone == K4ATrackedBoneLeftKnee) {
		m_sSerialNumber = "LKnee";
		m_sModelNumber = "K4A_DK_BNODE_LKNEE";
	}
}


vr::EVRInitError K4ATrackerDriver::Activate(vr::TrackedDeviceIndex_t unObjectId) {
	m_unObjectId = unObjectId;
	m_ulPropertyContainer = vr::VRProperties()->TrackedDeviceToPropertyContainer(m_unObjectId);

	if (m_bone == K4ATrackedBoneHip)
		m_bone_provider->setup_bone(unObjectId, K4ABT_JOINT_PELVIS);
	else if (m_bone == K4ATrackedBoneRightFoot)
		m_bone_provider->setup_bone(unObjectId, K4ABT_JOINT_FOOT_RIGHT);
	else if (m_bone == K4ATrackedBoneLeftFoot)
		m_bone_provider->setup_bone(unObjectId, K4ABT_JOINT_FOOT_LEFT);
	else if (m_bone == K4ATrackedBoneChest)
		m_bone_provider->setup_bone(unObjectId, K4ABT_JOINT_SPINE_CHEST);
	else if (m_bone == K4ATrackedBoneRightElbow)
		m_bone_provider->setup_bone(unObjectId, K4ABT_JOINT_ELBOW_RIGHT);
	else if (m_bone == K4ATrackedBoneLeftElbow)
		m_bone_provider->setup_bone(unObjectId, K4ABT_JOINT_ELBOW_LEFT);
	else if (m_bone == k4ATrackedBoneRightKnee)
		m_bone_provider->setup_bone(unObjectId, K4ABT_JOINT_KNEE_RIGHT);
	else if (m_bone == K4ATrackedBoneLeftKnee)
		m_bone_provider->setup_bone(unObjectId, K4ABT_JOINT_KNEE_LEFT);

	vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_ModelNumber_String, m_sModelNumber.c_str());
	vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_RenderModelName_String, "{k4a_openvr}/rendermodels/vr_tracker_vive_1_0/vr_tracker_vive_1_0.obj");

	vr::VRProperties()->SetUint64Property(m_ulPropertyContainer, vr::Prop_CurrentUniverseId_Uint64, 9834566758443);

	// this file tells the UI what to show the user for binding this controller as well as what default bindings should
	// be for legacy or other apps
	// vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_InputProfilePath_String, "{k4a_openvr}/resources/input/vive_tracker_profile.json");

	return vr::VRInitError_None;
}

vr::EVRInitError K4AServerDriver::Init(vr::IVRDriverContext* pDriverContext) {
	VR_INIT_SERVER_DRIVER_CONTEXT(pDriverContext);
	InitDriverLog(vr::VRDriverLog());

	m_bone_provider = new K4ABoneProvider(&DriverLog);

	m_bone_provider->Configure(K4A_DEPTH_MODE_WFOV_2X2BINNED, 0.075F);

	m_pHipTracker = new K4ATrackerDriver(K4ATrackedBone::K4ATrackedBoneHip, m_bone_provider);
	vr::VRServerDriverHost()->TrackedDeviceAdded(m_pHipTracker->GetSerialNumber().c_str(), vr::TrackedDeviceClass_GenericTracker, m_pHipTracker);

	m_pRightFootTracker = new K4ATrackerDriver(K4ATrackedBone::K4ATrackedBoneRightFoot, m_bone_provider);
	vr::VRServerDriverHost()->TrackedDeviceAdded(m_pRightFootTracker->GetSerialNumber().c_str(), vr::TrackedDeviceClass_GenericTracker, m_pRightFootTracker);

	m_pLeftFootTracker = new K4ATrackerDriver(K4ATrackedBone::K4ATrackedBoneLeftFoot, m_bone_provider);
	vr::VRServerDriverHost()->TrackedDeviceAdded(m_pLeftFootTracker->GetSerialNumber().c_str(), vr::TrackedDeviceClass_GenericTracker, m_pLeftFootTracker);

	m_pChestTracker = new K4ATrackerDriver(K4ATrackedBone::K4ATrackedBoneChest, m_bone_provider);
	vr::VRServerDriverHost()->TrackedDeviceAdded(m_pChestTracker->GetSerialNumber().c_str(), vr::TrackedDeviceClass_GenericTracker, m_pChestTracker);

	m_pRightElbowTracker = new K4ATrackerDriver(K4ATrackedBone::K4ATrackedBoneRightElbow, m_bone_provider);
	vr::VRServerDriverHost()->TrackedDeviceAdded(m_pRightElbowTracker->GetSerialNumber().c_str(), vr::TrackedDeviceClass_GenericTracker, m_pRightElbowTracker);

	m_pLeftElbowTracker = new K4ATrackerDriver(K4ATrackedBone::K4ATrackedBoneLeftElbow, m_bone_provider);
	vr::VRServerDriverHost()->TrackedDeviceAdded(m_pLeftElbowTracker->GetSerialNumber().c_str(), vr::TrackedDeviceClass_GenericTracker, m_pLeftElbowTracker);

	m_pRightKneeTracker = new K4ATrackerDriver(K4ATrackedBone::k4ATrackedBoneRightKnee, m_bone_provider);
	vr::VRServerDriverHost()->TrackedDeviceAdded(m_pRightKneeTracker->GetSerialNumber().c_str(), vr::TrackedDeviceClass_GenericTracker, m_pRightKneeTracker);

	m_pLeftKneeTracker = new K4ATrackerDriver(K4ATrackedBone::K4ATrackedBoneLeftKnee, m_bone_provider);
	vr::VRServerDriverHost()->TrackedDeviceAdded(m_pLeftKneeTracker->GetSerialNumber().c_str(), vr::TrackedDeviceClass_GenericTracker, m_pLeftKneeTracker);

	// SteamVR really doesn't like long ops on its child threads
	m_bone_started = std::async(std::launch::async, [&] { m_bone_provider->Start(); });

	return vr::VRInitError_None;
}

void K4AServerDriver::Cleanup() {
	DriverLog("Stopping K4AServerDriver\n");

	DriverLog("Last error: %d", m_bone_provider->GetLastError());
}

void K4AServerDriver::PowerOff()
{
	m_bone_provider->Stop();
	m_bone_provider->~K4ABoneProvider();
}