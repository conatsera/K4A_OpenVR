#ifndef BONECALIBRATORWINDOW_H
#define BONECALIBRATORWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class BoneCalibratorWindow; }
QT_END_NAMESPACE

const double pi = std::acos(-1);

const double degToRad = pi / 180;

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

typedef struct Quaternion
{
	float w;
	float x;
	float y;
	float z;
} Quaternion_t;

inline Quaternion_t QuaternionProduct(Quaternion_t& quata, Quaternion_t& quatb)
{
	Quaternion_t quat;
	quat.w = ((quata.w * quatb.w) - (quata.x * quatb.x) - (quata.y * quatb.y) - (quata.z * quatb.z));
	quat.x = ((quata.w * quatb.x) + (quata.x * quatb.w) + (quata.y * quatb.z) - (quata.z * quatb.y));
	quat.y = ((quata.w * quatb.y) - (quata.x * quatb.z) + (quata.y * quatb.w) + (quata.z * quatb.x));
	quat.z = ((quata.w * quatb.z) + (quata.x * quatb.y) - (quata.y * quatb.x) + (quata.z * quatb.w));
	return quat;
}


class BoneCalibratorWindow : public QMainWindow
{
	Q_OBJECT

public:
	BoneCalibratorWindow(QWidget* parent = nullptr);
	~BoneCalibratorWindow();

	void setOffsetMem(calibration_data_t* calibrationMem = nullptr);

private slots:
	void on_xOffset_valueChanged(int value);

	void on_yOffset_valueChanged(int value);

	void on_zOffset_valueChanged(int value);

	void on_rotX_valueChanged(int value);

	void on_rotY_valueChanged(int value);

	void on_rotZ_valueChanged(int value);

	void on_updateCheckBox_toggled(bool checked);

private:
	Ui::BoneCalibratorWindow* ui;

	int m_rotStackIndex = 0;
	calibration_data_t* m_calibration = nullptr;

	void UpdateRotQuat();
	Quaternion_t rotQuat;
	Quaternion_t adj_rot_quat_x;
	Quaternion_t adj_rot_quat_y;
	Quaternion_t adj_rot_quat_z;

};
#endif // BONECALIBRATORWINDOW_H
