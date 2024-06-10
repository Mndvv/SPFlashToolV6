#include "PrintDriverVersion.h"
#include "../Logger/Log.h"

#pragma comment(lib, "Setupapi.lib")

void PrintDriverVersion::run()
{
    this->print_driver_version();
}

void PrintDriverVersion::print_driver_version()
{
    GUID guid = {0x4d36e978, 0xe325, 0x11ce, {0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18}}; //comport

    HDEVINFO DeviceInfoSet = SetupDiGetClassDevs(
                                    &guid,
                                    NULL,
                                    NULL,
                                    DIGCF_PRESENT);

    SP_DEVINFO_DATA DeviceInfoData;
    ZeroMemory(&DeviceInfoData, sizeof(SP_DEVINFO_DATA));
    DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
    unsigned int DeviceIndex = 0;

    while (SetupDiEnumDeviceInfo(
                             DeviceInfoSet,
                             DeviceIndex,
                             &DeviceInfoData))
   {
        DeviceIndex++;

        SP_DRVINFO_DATA drvInfo;
        drvInfo.cbSize = sizeof(SP_DRVINFO_DATA);


        // Build a list of driver info items that we will retrieve below
        if (!SetupDiBuildDriverInfoList(DeviceInfoSet,
                                        &DeviceInfoData, SPDIT_COMPATDRIVER))
            return; // Exit on error

        if (!SetupDiEnumDriverInfo(DeviceInfoSet, &DeviceInfoData, SPDIT_COMPATDRIVER, 0, &drvInfo))
        {
            int err_code = GetLastError();
            LOGD("err : %d\n", err_code);
        }
        else
        {

            //find device by PID/VID, support 0e8d_0003(bootrom), 0e8d_2000(preloader)
            if(find_usbdev(DeviceInfoSet, &DeviceInfoData, "0e8d", "0003")
                || find_usbdev(DeviceInfoSet, &DeviceInfoData, "0e8d", "2000"))
            {
                unsigned high = (unsigned)(drvInfo.DriverVersion >> 32);
                unsigned low = (unsigned)(drvInfo.DriverVersion & 0xffffffffULL);
                LOGD("Driver version is %d.%d.%d.%d",
                       (high >> 16),(high & 0xffffu),(low >> 16),(low & 0xffffu));
            }
        }

    }

    if (DeviceInfoSet)
    {
        SetupDiDestroyDeviceInfoList(DeviceInfoSet);
    }
}

bool PrintDriverVersion::find_usbdev(HDEVINFO DeviceInfoSet, SP_DEVINFO_DATA *DeviceInfoData, const QString vid, const QString &pid)
{
    char szDeviceInstanceID[MAX_PATH] = {0};

    BOOL r = SetupDiGetDeviceInstanceIdA(DeviceInfoSet, DeviceInfoData, szDeviceInstanceID, MAX_PATH, NULL);
    QString device_instance_id = QString::fromStdString(std::string(szDeviceInstanceID));
    if(r && device_instance_id.indexOf(vid, 0, Qt::CaseInsensitive) != -1
            && device_instance_id.indexOf(pid, 0, Qt::CaseInsensitive) != -1)
    {
        LOGD("szDeviceInstanceID : %s", szDeviceInstanceID);
        return true;
    }
    else
       return false;
}
