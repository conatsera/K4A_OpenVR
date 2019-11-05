#include "bone_calibrator_window.h"
#include "./ui_bone_calibrator_window.h"

BoneCalibratorWindow::BoneCalibratorWindow(QWidget* parent)
	: QMainWindow(parent)
	, ui(new Ui::BoneCalibratorWindow)
{
	Quaternion_t zeroQuat;
	zeroQuat.w = 1.F;
	zeroQuat.x = 0.F;
	zeroQuat.y = 0.F;
	zeroQuat.z = 0.F;

	rotQuat = zeroQuat;
	adj_rot_quat_x = zeroQuat;
	adj_rot_quat_y = zeroQuat;
	adj_rot_quat_z = zeroQuat;

	ui->setupUi(this);
}

BoneCalibratorWindow::~BoneCalibratorWindow()
{
	delete ui;
}

void BoneCalibratorWindow::setOffsetMem(calibration_data_t* offsetMem)
{
	m_calibration = offsetMem;
}

void BoneCalibratorWindow::on_xOffset_valueChanged(int value)
{
	m_calibration->x = value / 1000.0F;
}

void BoneCalibratorWindow::on_yOffset_valueChanged(int value)
{
	m_calibration->y = value / 1000.0F;
}

void BoneCalibratorWindow::on_zOffset_valueChanged(int value)
{
	m_calibration->z = value / 1000.0F;
}

void BoneCalibratorWindow::UpdateRotQuat()
{
	rotQuat.w = 1.F;
	rotQuat.x = 0.F;
	rotQuat.y = 0.F;
	rotQuat.z = 0.F;

	if (adj_rot_quat_x.x != 0.F)
		rotQuat = QuaternionProduct(rotQuat, adj_rot_quat_x);

	if (adj_rot_quat_y.y != 0.F)
		rotQuat = QuaternionProduct(rotQuat, adj_rot_quat_y);

	if (adj_rot_quat_z.z != 0.F)
		rotQuat = QuaternionProduct(rotQuat, adj_rot_quat_z);

	m_calibration->rotOffset.w = rotQuat.w;
	m_calibration->rotOffset.x = rotQuat.x;
	m_calibration->rotOffset.y = rotQuat.y;
	m_calibration->rotOffset.z = rotQuat.z;
}

void BoneCalibratorWindow::on_rotX_valueChanged(int value)
{
	float rads = (value * 45) * degToRad;

	adj_rot_quat_x.w = std::cos(rads);
	adj_rot_quat_x.x = std::sin(rads);
	adj_rot_quat_x.y = 0.F;
	adj_rot_quat_x.z = 0.F;

	UpdateRotQuat();
}

void BoneCalibratorWindow::on_rotY_valueChanged(int value)
{
	float rads = (value * 45) * degToRad;

	adj_rot_quat_y.w = std::cos(rads);
	adj_rot_quat_y.x = 0.F;
	adj_rot_quat_y.y = std::sin(rads);
	adj_rot_quat_y.z = 0.F;

	UpdateRotQuat();
}

void BoneCalibratorWindow::on_rotZ_valueChanged(int value)
{
	float rads = (value * 45) * degToRad;

	adj_rot_quat_z.w = std::cos(rads);
	adj_rot_quat_z.x = 0.F;
	adj_rot_quat_z.y = 0.F;
	adj_rot_quat_z.z = std::sin(rads);

	UpdateRotQuat();
}


void BoneCalibratorWindow::on_updateCheckBox_toggled(bool checked)
{
	m_calibration->update = checked;
}
