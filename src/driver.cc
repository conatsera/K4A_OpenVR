#include "driver.h"

namespace K4AOpenVR
{

TrackedJointDriver::TrackedJointDriver(k4abt_joint_id_t jointId) : m_jointId(jointId)
{
    switch (jointId)
    {
    case k4abt_joint_id_t::K4ABT_JOINT_PELVIS:
        m_sModelName = "K4ABT_JOINT_PELVIS";
        break;
    case k4abt_joint_id_t::K4ABT_JOINT_ELBOW_LEFT:
        m_sModelName = "K4ABT_JOINT_ELBOW_LEFT";
        break;
    case k4abt_joint_id_t::K4ABT_JOINT_HAND_LEFT:
        m_sModelName = "K4ABT_JOINT_HAND_LEFT";
        break;
    case k4abt_joint_id_t::K4ABT_JOINT_ELBOW_RIGHT:
        m_sModelName = "K4ABT_JOINT_ELBOW_RIGHT";
        break;
    case k4abt_joint_id_t::K4ABT_JOINT_HAND_RIGHT:
        m_sModelName = "K4ABT_JOINT_HAND_RIGHT";
        break;
    case k4abt_joint_id_t::K4ABT_JOINT_KNEE_LEFT:
        m_sModelName = "K4ABT_JOINT_KNEE_LEFT";
        break;
    case k4abt_joint_id_t::K4ABT_JOINT_FOOT_LEFT:
        m_sModelName = "K4ABT_JOINT_FOOT_LEFT";
        break;
    case k4abt_joint_id_t::K4ABT_JOINT_KNEE_RIGHT:
        m_sModelName = "K4ABT_JOINT_KNEE_RIGHT";
        break;
    case k4abt_joint_id_t::K4ABT_JOINT_FOOT_RIGHT:
        m_sModelName = "K4ABT_JOINT_FOOT_RIGHT";
        break;
    case k4abt_joint_id_t::K4ABT_JOINT_HEAD:
        m_sModelName = "K4ABT_JOINT_HEAD";
        break;
    }
}

vr::EVRInitError TrackedJointDriver::Activate(vr::TrackedDeviceIndex_t unObjectId)
{
    m_unObjectId = unObjectId;
    m_ulPropertyContainer = vr::VRProperties()->TrackedDeviceToPropertyContainer(unObjectId);

    vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_ModelNumber_String, m_sModelName.c_str());
    vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_RenderModelName_String, "locator");

    vr::VRProperties()->SetUint64Property(m_ulPropertyContainer, vr::Prop_CurrentUniverseId_Uint64, 9834566758443);

    return vr::EVRInitError::VRInitError_None;
}

void TrackedJointDriver::Deactivate()
{
    m_unObjectId = vr::k_unTrackedDeviceIndexInvalid;
}

void TrackedJointDriver::EnterStandby()
{
}

void *TrackedJointDriver::GetComponent(const char *)
{
    return nullptr;
}

void TrackedJointDriver::DebugRequest(const char *, char *pchResponseBuffers, uint32_t unResponseBufferSize)
{
    if (unResponseBufferSize > 0)
        pchResponseBuffers[0] = '\0';
}

} // namespace K4AOpenVR