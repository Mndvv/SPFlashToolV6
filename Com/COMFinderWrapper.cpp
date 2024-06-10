#include "COMFinderWrapper.h"
#include <QSerialPortInfo>
#include <QSettings>
#include <QRegExp>
#include <algorithm>

static bool comportCompare(const QString &com1, const QString &com2)
{
    int len1 = com1.length();
    int len2 = com2.length();
    if (len1 == len2) {
        int com_num1 = com1.mid(com1.indexOf(QRegExp("\\d"))).toInt();
        int com_num2 = com2.mid(com2.indexOf(QRegExp("\\d"))).toInt();
        return com_num1 < com_num2;
    } else {
        return len1 < len2;
    }
}

QList<QString> COMFinderWrapper::getComPortInfoStrs()
{
    QList<QString> com_port_list;
    foreach (auto com_port_info, QSerialPortInfo::availablePorts()) {
        com_port_list.push_back(com_port_info.portName());
    }
    std::sort(com_port_list.begin(), com_port_list.end(), comportCompare);
    return com_port_list;
}
