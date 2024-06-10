#ifndef TYPES_H
#define TYPES_H

#include <functional>
#include <QString>

using CB_OPERATION_PROGRESS_FUNC = std::function<void (void *_this,
    unsigned int progress, unsigned long long data_xferd, const QString &info)>;

using CB_COM_PORT_STATUS_FUNC = std::function<void (const std::string &friendly_port_name)>;
using CB_DA_CONNECTED_FUNC = std::function<void (const QString &da_conn_xml)>;
using CB_DEVICE_TRACKING_QUERY_FUNC = std::function<void (int *p_out)>;

#endif // TYPES_H
