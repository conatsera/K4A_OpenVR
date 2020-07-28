#include "k4abt.h"
#include "SimpleKalmanFilter.h"

#ifndef bone_filter_h
#define bone_filter_h

class bone_filter {
public:
	// Give raw position data, get estimated position
	k4abt_joint_t getNextPos(k4abt_joint_t);
	
	// set the measurement error of all 3 axis
	void setMeasurementError(float mea_e);

	// set the estimate error of all 3 axis
	void setEstimateError(float est_e);

	// set the processNoise of all 3 axis
	void setProcessNoise(float q);

private:
	SimpleKalmanFilter x = SimpleKalmanFilter(0.2, 0.1, 0.01);
	SimpleKalmanFilter y = SimpleKalmanFilter(0.2, 0.1, 0.01);
	SimpleKalmanFilter z = SimpleKalmanFilter(0.2, 0.1, 0.01);
};





#endif