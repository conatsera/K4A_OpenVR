#include "bone_calibrator_window.h"

#include <iostream>

#include <QApplication>
#include <Windows.h>

static TCHAR szK4ACalibration[] = TEXT("BoneCalibrationMemmap");
#define	CALIBRATION_MEMSIZE (sizeof(calibration_data_t))

static calibration_data_t* lpvK4ACalibrationMem = nullptr;
static HANDLE hK4ACalibrationMapObject = nullptr;

int main(int argc, char* argv[])
{
	hK4ACalibrationMapObject = OpenFileMapping(
		FILE_MAP_ALL_ACCESS,
		FALSE,
		szK4ACalibration
	);

	if (hK4ACalibrationMapObject == nullptr)
	{
		std::cout << "Could not open file mapping object " << GetLastError() << std::endl;
		return -2;
	}

	lpvK4ACalibrationMem = reinterpret_cast<calibration_data_t*>((LPTSTR)MapViewOfFile(hK4ACalibrationMapObject,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		CALIBRATION_MEMSIZE));

	if (lpvK4ACalibrationMem == nullptr)
	{
		std::cout << "Could not map view of file " << GetLastError() << std::endl;

		CloseHandle(hK4ACalibrationMapObject);
		return -2;
	}

	QApplication a(argc, argv);
	BoneCalibratorWindow w;
	w.setOffsetMem(lpvK4ACalibrationMem);
	w.show();
	return a.exec();
}
