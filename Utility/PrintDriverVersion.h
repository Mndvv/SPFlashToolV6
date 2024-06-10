#ifndef PRINTDRIVERVERSION_H
#define PRINTDRIVERVERSION_H

#include <Windows.h>
#include <SetupAPI.h>
#include <QRunnable>

class PrintDriverVersion: public QRunnable
{
public:
    virtual void run() override;

private:
    void print_driver_version();
    bool find_usbdev(HDEVINFO DeviceInfoSet, SP_DEVINFO_DATA *DeviceInfoData, const QString vid, const QString &pid);
};

#endif // PRINTDRIVERVERSION_H
