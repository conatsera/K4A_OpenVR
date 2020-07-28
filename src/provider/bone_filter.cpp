#include "bone_filter.h"

k4abt_joint_t bone_filter::getNextPos(k4abt_joint_t rawJoint) {
	k4abt_joint_t predictedJoint;
	predictedJoint.position.xyz.x = x.updateEstimate(rawJoint.position.xyz.x);
	predictedJoint.position.xyz.y = y.updateEstimate(rawJoint.position.xyz.y);
	predictedJoint.position.xyz.z = z.updateEstimate(rawJoint.position.xyz.z);
	return predictedJoint;
}

void bone_filter::setEstimateError(float est_e) {
	x.setEstimateError(est_e);
	y.setEstimateError(est_e);
	z.setEstimateError(est_e);
}

void bone_filter::setMeasurementError(float mea_e) {
	x.setMeasurementError(mea_e);
	y.setMeasurementError(mea_e);
	z.setMeasurementError(mea_e);
}

void bone_filter::setProcessNoise(float q) {
	x.setProcessNoise(q);
	y.setProcessNoise(q);
	z.setProcessNoise(q);
}