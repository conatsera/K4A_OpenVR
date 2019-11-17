#ifndef BONECALIBRATORWINDOW_H
#define BONECALIBRATORWINDOW_H

#include <QMainWindow>

#include <QMap>
#include <QSettings>

QT_BEGIN_NAMESPACE
namespace Ui { class BoneCalibratorWindow; }
QT_END_NAMESPACE

const float pi = (float)std::acos(-1);

const float degToRad = pi / 180;

typedef struct Quaternion
{
    float w;
    float x;
    float y;
    float z;
} Quaternion_t;

typedef struct _calibration_data
{
	bool update;

	float x;
	float y;
	float z;

    Quaternion_t rotOffset;
} calibration_data_t;

typedef struct _calibration_preset
{
    int x;
    int y;
    int z;

    int roll;
    int pitch;
    int yaw;
} calibration_preset_t;

const Quaternion_t zeroQuat{ 1.F, 0.F, 0.F, 0.F };
const Quaternion_t def_rot_quat_x{ (float)std::cos(std::acos(-1) / 4), (float)std::sin(std::acos(-1) / 4), 0.F, 0.F };

inline Quaternion_t QuaternionProduct(const Quaternion_t& quata, const Quaternion_t& quatb)
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
	BoneCalibratorWindow(QWidget* parent = nullptr, calibration_data_t* offsetMem = nullptr);
	~BoneCalibratorWindow();

private slots:
	void on_xOffset_valueChanged(int value);

	void on_yOffset_valueChanged(int value);

	void on_zOffset_valueChanged(int value);

	void on_rotX_valueChanged(int value);

	void on_rotY_valueChanged(int value);

	void on_rotZ_valueChanged(int value);

	void on_updateCheckBox_toggled(bool checked);

    void on_actionAdd_Preset_triggered();

    void on_actionSet_Default_triggered();

private:
	Ui::BoneCalibratorWindow* ui;

	int m_rotStackIndex = 0;
	calibration_data_t* m_calibration = nullptr;

	void UpdateRotQuat();
	Quaternion_t rotQuat = zeroQuat;
	Quaternion_t adj_rot_quat_x = def_rot_quat_x;
	Quaternion_t adj_rot_quat_y = zeroQuat;
	Quaternion_t adj_rot_quat_z = zeroQuat;

    QSettings m_settings{"presets.ini", QSettings::Format::IniFormat};
    void SavePresets() const;
    void LoadPresets();

    void SetPreset(const calibration_preset_t preset);
	QString current_preset_name = "";
	calibration_preset_t current_preset{ 0 };
    QMap<QString, calibration_preset_t> m_presets;
};
#endif // BONECALIBRATORWINDOW_H
