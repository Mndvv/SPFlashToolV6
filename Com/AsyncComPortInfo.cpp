#include "AsyncComPortInfo.h"
#include "COMFinderWrapper.h"

AsyncComPortInfo::AsyncComPortInfo()
{

}

AsyncComPortInfo::~AsyncComPortInfo()
{

}

void AsyncComPortInfo::run()
{
    QList<QString> com_port_list = COMFinderWrapper::getComPortInfoStrs();
    emit signal_send_com_port_infos(com_port_list);
}
