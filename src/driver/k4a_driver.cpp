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

	vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_ModelNumber_String, m_sModelNumber.c_str());
	vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_RenderModelName_String, "Kinect for Azure Bone Node");

	vr::VRProperties()->SetUint64Property(m_ulPropertyContainer, vr::Prop_CurrentUniverseId_Uint64, 9834566758443);

	// this file tells the UI what to show the user for binding this controller as well as what default bindings should
	// be for legacy or other apps
	// TODO: vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_InputProfilePath_String, "{sample}/input/mycontroller_profile.json");

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