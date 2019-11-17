#include "new_profile_dialog.h"
#include "bone_calibrator_window.h"
#include "./ui_bone_calibrator_window.h"

BoneCalibratorWindow::BoneCalibratorWindow(QWidget* parent, calibration_data_t* offsetMem)
	: QMainWindow(parent)
	, ui(new Ui::BoneCalibratorWindow)
	, m_calibration(offsetMem)
{
	ui->setupUi(this);
	LoadPresets();
}

BoneCalibratorWindow::~BoneCalibratorWindow()
{
	delete ui;
}

void BoneCalibratorWindow::on_xOffset_valueChanged(int value)
{
    current_preset.x = value;
	m_calibration->x = value / 1000.0F;
}

void BoneCalibratorWindow::on_yOffset_valueChanged(int value)
{
    current_preset.y = value;
	m_calibration->y = value / 1000.0F;
}

void BoneCalibratorWindow::on_zOffset_valueChanged(int value)
{
    current_preset.z = value;
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
    current_preset.roll = value;
    float rads = (value / 2.F) * degToRad;

	adj_rot_quat_x.w = std::cos(rads);
	adj_rot_quat_x.x = std::sin(rads);
	adj_rot_quat_x.y = 0.F;
	adj_rot_quat_x.z = 0.F;

	adj_rot_quat_x = QuaternionProduct(def_rot_quat_x, adj_rot_quat_x);

	UpdateRotQuat();
}

void BoneCalibratorWindow::on_rotY_valueChanged(int value)
{
    current_preset.pitch = value;
    float rads = (value / 2.F) * degToRad;

	adj_rot_quat_y.w = std::cos(rads);
	adj_rot_quat_y.x = 0.F;
	adj_rot_quat_y.y = std::sin(rads);
	adj_rot_quat_y.z = 0.F;

	UpdateRotQuat();
}

void BoneCalibratorWindow::on_rotZ_valueChanged(int value)
{
    current_preset.yaw = value;
    float rads = (value / 2.F) * degToRad;

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


inline QList<QVariant> PresetToQList(const calibration_preset_t& preset) {
    QList<QVariant> presetList{
        preset.x,
        preset.y,
        preset.z,
        preset.roll,
        preset.pitch,
        preset.yaw,
    };
    return presetList;
}

inline calibration_preset_t QListToPreset(const QList<QVariant>& presetList) {
    calibration_preset_t preset{
        presetList[0].toInt(),
        presetList[1].toInt(),
        presetList[2].toInt(),
        presetList[3].toInt(),
        presetList[4].toInt(),
        presetList[5].toInt(),
    };
    return preset;
}

void BoneCalibratorWindow::LoadPresets() {
	QString default_preset = m_settings.value("default").toString();
	m_settings.beginGroup("Presets");
    QStringList preset_names = m_settings.childKeys();
    for (const auto& preset_name : preset_names) {
        calibration_preset_t preset = QListToPreset(m_settings.value(preset_name).toList());
        m_presets.insert(preset_name, preset);

		ui->menuPresets->addAction(preset_name, [this, preset_name] {
			if (m_presets.count(preset_name) > 0) {
				current_preset_name = preset_name;
				SetPreset(m_presets.value(preset_name));
			}
			});

        ui->menuRemove_Preset->addAction(preset_name, [this, preset_name]{
           if (m_presets.count(preset_name) > 0)
               m_presets.remove(preset_name);

		   if (m_settings.value("default").toString() == preset_name)
			   m_settings.remove("default");

		   if (current_preset_name == preset_name)
			   current_preset_name = "";

		   m_settings.beginGroup("Presets");
		   m_settings.remove(preset_name);
		   m_settings.endGroup();

           for (QAction* profileAction : ui->menuPresets->actions()) {
               if (profileAction->text() == preset_name) {
				   QMenu* profileActionMenu = (QMenu*)profileAction->parent();
				   profileActionMenu->removeAction(profileAction);
			   }
           }
		   for (QAction* profileAction : ui->menuRemove_Preset->actions()) {
			   if (profileAction->text() == preset_name) {
				   QMenu* profileActionMenu = (QMenu*)profileAction->parent();
				   profileActionMenu->removeAction(profileAction);
			   }
		   }
        });

		if (preset_name == default_preset) {
			current_preset_name = preset_name;
			SetPreset(preset);
		}
    }
	m_settings.endGroup();


}

void BoneCalibratorWindow::SetPreset(const calibration_preset_t preset)
{
    ui->xOffset->setValue(preset.x);
    ui->yOffset->setValue(preset.y);
    ui->zOffset->setValue(preset.z);
    ui->rotX->setValue(preset.roll);
    ui->rotY->setValue(preset.pitch);
    ui->rotZ->setValue(preset.yaw);
}

void BoneCalibratorWindow::on_actionAdd_Preset_triggered()
{
    NewProfileDialog newProfileDialog(this);
    if (QDialog::Accepted == newProfileDialog.exec()) {
        QString newProfileName = newProfileDialog.GetNewProfileName();

        m_presets.insert(newProfileName, current_preset);

		m_settings.beginGroup("Presets");
        m_settings.setValue(newProfileName, QVariant(PresetToQList(current_preset)));
		m_settings.endGroup();

        ui->menuPresets->addAction(newProfileName, [this, newProfileName]{
			if (m_presets.count(newProfileName) > 0) {
				current_preset_name = newProfileName;
				SetPreset(m_presets.value(newProfileName));
			}
        });

        ui->menuRemove_Preset->addAction(newProfileName, [this, newProfileName]{
           if (m_presets.count(newProfileName) > 0)
               m_presets.remove(newProfileName);

		   if (m_settings.value("default").toString() == newProfileName)
			   m_settings.remove("default");

		   if (current_preset_name == newProfileName)
			   current_preset_name = "";

		   m_settings.beginGroup("Presets");
		   m_settings.remove(newProfileName);
		   m_settings.endGroup();

		   for (QAction* profileAction : ui->menuPresets->actions()) {
			   if (profileAction->text() == newProfileName) {
				   QMenu* profileActionMenu = (QMenu*)profileAction->parent();
				   profileActionMenu->removeAction(profileAction);
			   }
		   }
		   for (QAction* profileAction : ui->menuRemove_Preset->actions()) {
			   if (profileAction->text() == newProfileName) {
				   QMenu* profileActionMenu = (QMenu*)profileAction->parent();
				   profileActionMenu->removeAction(profileAction);
			   }
		   }
        });
    }
}

void BoneCalibratorWindow::on_actionSet_Default_triggered()
{
	m_settings.setValue("default", current_preset_name);
}
