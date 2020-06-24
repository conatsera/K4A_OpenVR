#ifndef K4A_OPENVR_CALIBRATOR_CALIBRATION_H
#define K4A_OPENVR_CALIBRATOR_CALIBRATION_H

#include "imgui.h"
#include <openvr.h>

namespace Calibration {
	enum State {
		SUCCESS = 0
	};


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

		float x;
		float y;
		float z;

		joint_offset_t rotOffset;
	} calibration_data_t;

	class Calibrator {
	public:
		Calibrator();
		~Calibrator();

		const State UpdateActive() const;
		const State Calibrate();

	};
} // namespace Calibration

#endif