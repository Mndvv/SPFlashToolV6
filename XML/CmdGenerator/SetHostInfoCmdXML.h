#ifndef SETHOSTINFOCMDXMLGENERATOR_H
#define SETHOSTINFOCMDXMLGENERATOR_H

#include "ICmdXMLGenerator.h"

class SetHostInfoCmdXML : public ICmdXMLGenerator
{
public:
    SetHostInfoCmdXML();

    XML_CMD_GENERATOR_VIRTUAL_FUNCS

    void setInfo(const QString &info);

private:
    QString m_info;
};

#endif // SETHOSTINFOCMDXMLGENERATOR_H
