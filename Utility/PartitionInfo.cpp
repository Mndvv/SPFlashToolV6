#include "PartitionInfo.h"
#include "../../Inc/CFlashToolConst.h"

PartitionInfo::PartitionInfo():
    m_enabled(false),
    m_startAddress(QStringLiteral("0x0000000000000000"))
{

}

QString PartitionInfo::file_name() const
{
    return m_file_name;
}

void PartitionInfo::setFile_name(const QString &file_name)
{
    m_file_name = file_name;
}

QString PartitionInfo::partition_name() const
{
    return m_partition_name;
}

void PartitionInfo::setPartition_name(const QString &partition_name)
{
    m_partition_name = partition_name;
}

QString PartitionInfo::startAddress() const
{
    return m_startAddress;
}

void PartitionInfo::setStartAddress(const QString &startAddress)
{
    m_startAddress = startAddress;
}

bool PartitionInfo::enabled() const
{
    return m_enabled;
}

void PartitionInfo::setEnabled(bool enabled)
{
    m_enabled = enabled;
}

ReadbackPartitionInfo::ReadbackPartitionInfo():
    m_length(QStringLiteral("0x0000000000001000")),
    m_region_text(UFS_LUA2_VAL)
{

}

void ReadbackPartitionInfo::load(const QDomElement &element_node)
{
    m_enabled = element_node.attribute(QStringLiteral("enabled"), QStringLiteral("false")).toLower() == QStringLiteral("true");
    m_partition_name = element_node.attribute(QStringLiteral("partition-name"));
    m_startAddress = element_node.attribute(QStringLiteral("start-addr"));
    m_length = element_node.attribute(QStringLiteral("length"));
    m_region_text = element_node.attribute(QStringLiteral("region-text"));
    m_file_name = element_node.text().trimmed();
}

void ReadbackPartitionInfo::save(QDomDocument &xml_doc, QDomElement &element_node) const
{
    QDomElement readback_item_node = xml_doc.createElement(QStringLiteral("readback-item"));
    element_node.appendChild(readback_item_node);

    readback_item_node.setAttribute(QStringLiteral("enabled"), m_enabled ? QStringLiteral("true") : QStringLiteral("false"));
    readback_item_node.setAttribute(QStringLiteral("partition-name"), m_partition_name);
    readback_item_node.setAttribute(QStringLiteral("start-addr"), m_startAddress);
    readback_item_node.setAttribute(QStringLiteral("length"), m_length);
    readback_item_node.setAttribute(QStringLiteral("region-text"), m_region_text);
    QDomText text_node = xml_doc.createTextNode(m_file_name);
    readback_item_node.appendChild(text_node);
}

QString ReadbackPartitionInfo::length() const
{
    return m_length;
}

void ReadbackPartitionInfo::setLength(const QString &length)
{
    m_length = length;
}

QString ReadbackPartitionInfo::region_region() const
{
    return m_region_text;
}

void ReadbackPartitionInfo::setRegion_text(const QString &region_text)
{
    m_region_text = region_text;
}
