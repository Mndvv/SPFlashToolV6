#include "SetHostInfoCmdXML.h"
#include "../../XML/XMLConst.h"

SetHostInfoCmdXML::SetHostInfoCmdXML():
    ICmdXMLGenerator(SET_HOST_INFO_CMD_VER_VAL, SET_HOST_INFO_CMD_NAME_VAL, DA_NODE_NAME)
{

}

void SetHostInfoCmdXML::setInfo(const QString &info)
{
    m_info = info;
}

/*
<?xml version="1.0" encoding="utf-8"?>
<da>
    <version>1.0</version>
    <command>CMD:SET-HOST-INFO</command>
    <arg>
        <info>2002.02.03T11.22.33-PC11123445</info>
    </arg>
</da>
*/
void SetHostInfoCmdXML::fillArgNode(QDomElement * const arg_node)
{
    assert(nullptr != arg_node);
    this->createXMLSubNode(arg_node, INFO_NODE_NAME, m_info);
}
