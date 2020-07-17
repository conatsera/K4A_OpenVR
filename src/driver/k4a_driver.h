// K4A OpenVR.h : Include file for standard system include files,
// or project specific include files.

#pragma once
#ifndef K4A_OPENVR_DRIVER_H
#define K4A_OPENVR_DRIVER_H

#include <thread>
#include <future>

#include "log.h"
#include "k4abttypes.h"
#include "openvr_driver.h"

#include "provider/bone_provider.h"

inline vr::HmdQuaternion_t QuaternionInverse(k4a_quaternion_t::_wxyz& quat)
{
	vr::HmdQuaternion_t inverse_quat;
	inverse_quat.w = quat.w;
	inverse_quat.x = -(quat.x);
	inverse_quat.y = -(quat.y);
	inverse_quat.z = -(quat.z);
	return inverse_quat;
}

inline vr::HmdQuaternion_t HmdQuaternion_Init(double w, double x, double y, double z)
{
	vr::HmdQuaternion_t quat;
	quat.w = w;
	quat.x = x;
	quat.y = y;
	quat.z = z;
	return quat;
}

inline void HmdMatrix_SetIdentity(vr::HmdMatrix34_t* pMatrix)
{
	pMatrix->m[0][0] = 1.f;
	pMatrix->m[0][1] = 0.f;
	pMatrix->m[0][2] = 0.f;
	pMatrix->m[0][3] = 0.f;
	pMatrix->m[1][0] = 0.f;
	pMatrix->m[1][1] = 1.f;
	pMatrix->m[1][2] = 0.f;
	pMatrix->m[1][3] = 0.f;
	pMatrix->m[2][0] = 0.f;
	pMatrix->m[2][1] = 0.f;
	pMatrix->m[2][2] = 1.f;
	pMatrix->m[2][3] = 0.f;
}

class K4AWatchdogDriver : public vr::IVRWatchdogProvider {
public:
	K4AWatchdogDriver() {
		m_pWatchdogThread = nullptr;
	}

	virtual vr::EVRInitError Init(vr::IVRDriverContext* pDriverContext);
	virtual void Cleanup();
private:
	std::thread* m_pWatchdogThread;
};

enum K4ATrackedBone {
	K4ATrackedBoneInvalid = 0,
	K4ATrackedBoneHip = 1,
	K4ATrackedBoneRightFoot = 2,
	K4ATrackedBoneLeftFoot = 3,
	K4ATrackedBoneRightHand = 4,
	K4ATrackedBoneLeftHand = 5,
	K4ATrackedBoneHead = 6,
	K4ATrackedBoneChest = 7,
	K4ATrackedBoneRightElbow = 8,
	K4ATrackedBoneLeftElbow = 9,
	k4ATrackedBoneRightKnee = 10,
	K4ATrackedBoneLeftKnee = 11
};

class K4ATrackerDriver : public vr::ITrackedDeviceServerDriver {
public:
	K4ATrackerDriver(K4ATrackedBone bone, K4ABoneProvider* bone_provider);
	virtual ~K4ATrackerDriver() {};

	virtual vr::EVRInitError Activate(vr::TrackedDeviceIndex_t unObjectId);
	virtual void Deactivate() {
		m_unObjectId = vr::k_unTrackedDeviceIndexInvalid;
	};

	virtual void DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize) {
		if (unResponseBufferSize >= 1)
			pchResponseBuffer[0] = 0;
	};

	virtual vr::DriverPose_t GetPose() {
		return m_pose;
	};

	virtual bool ShouldBlockStandbyMode() { return false; };
	virtual void EnterStandby() { };
	virtual void LeaveStandby() { };

	void* GetComponent(const char* pchComponentNameAndVersion) {
		// override this to add a component to a driver
		return NULL;
	};

	//void ProcessEvent(const vr::VREvent_t &vrEvent);

	std::string GetSerialNumber() const { return m_sSerialNumber; };
private:
	K4ABoneProvider* m_bone_provider = nullptr;

	vr::TrackedDeviceIndex_t m_unObjectId;
	vr::PropertyContainerHandle_t m_ulPropertyContainer;

	std::string m_sSerialNumber;
	std::string m_sModelNumber;

	K4ATrackedBone m_bone;
	vr::DriverPose_t m_pose;
};

class K4AServerDriver : public vr::IServerTrackedDeviceProvider {
public:
	virtual vr::EVRInitError Init(vr::IVRDriverContext* pDriverContext);
	virtual void Cleanup();
	virtual const char* const* GetInterfaceVersions() { return vr::k_InterfaceVersions; };

	virtual void RunFrame() { };

	virtual bool ShouldBlockStandbyMode() { return false; };
	virtual void EnterStandby() { };
	virtual void LeaveStandby() { };


	virtual void PowerOff();

private:
	K4ABoneProvider* m_bone_provider = nullptr;

	K4ATrackerDriver* m_pHipTracker = nullptr;
	K4ATrackerDriver* m_pRightFootTracker = nullptr;
	K4ATrackerDriver* m_pLeftFootTracker = nullptr;
	K4ATrackerDriver* m_pChestTracker = nullptr;
	K4ATrackerDriver* m_pRightElbowTracker = nullptr;
	K4ATrackerDriver* m_pLeftElbowTracker = nullptr;
	K4ATrackerDriver* m_pRightKneeTracker = nullptr;
	K4ATrackerDriver* m_pLeftKneeTracker = nullptr;

	std::future<void> m_bone_started;
};


#endif