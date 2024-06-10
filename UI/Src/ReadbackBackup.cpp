#include "ReadbackBackup.h"
#include <QFile>
#include <QTextStream>
#include "../../Logger/Log.h"
#include "../../Utility/Utils.h"
#include <cassert>

ReadbackBackup::ReadbackBackup():
    m_storage_type(HW_StorageType_E::HW_STORAGE_NONE)
{

}

void ReadbackBackup::setChipName(const QString &chip_name)
{
    m_chip_name = chip_name;
}

void ReadbackBackup::setStorageType(HW_StorageType_E storage_type)
{
    m_storage_type = storage_type;
}

QString ReadbackBackup::getChipName() const
{
    return m_chip_name;
}

HW_StorageType_E ReadbackBackup::getStorageType() const
{
    return m_storage_type;
}

void ReadbackBackup::setRBManualList(const ReadbackPartitionInfoList &rb_list)
{
    m_rb_manual_list = rb_list;
}

void ReadbackBackup::setRBAutoList(const ReadbackPartitionInfoList &rb_list)
{
    m_rb_auto_list = rb_list;
}

bool ReadbackBackup::loadFile(const QString &file_name)
{
    if (file_name.isEmpty()) {
        LOG("load readback xml file failed: file name is empty.");
        return false;
    }

    QFile file(file_name);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        LOG("open readback xml file failed: %s.", file_name.toStdString().c_str());
        return false;
    }

    QDomDocument xml_dom_doc;
    QString errorStr;
    int errorLine;
    int errorColumn;
    if (!xml_dom_doc.setContent(&file, &errorStr, &errorLine, &errorColumn)) {
        LOG("load config xml file failed: error msg: %s, error line: %d, error column: %d",
            errorStr.toStdString().c_str(), errorLine, errorColumn);
        return false;
    }

    m_rb_manual_list.clear();
    m_rb_auto_list.clear();

    QDomElement root_node = xml_dom_doc.documentElement();
    assert(root_node.tagName() == QStringLiteral("readback-backup"));
    assert(root_node.attribute(QStringLiteral("version")) == QStringLiteral("1.0"));
    loadGeneral(root_node);
    loadRBInfo(root_node);
    return true;
}

void ReadbackBackup::saveFile(const QString &file_name) const
{
    if (file_name.isEmpty()) {
        LOG("save readback xml file failed: file name is empty.");
        return ;
    }
    QFile file(file_name);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        LOG("save readback xml file failed: %s open failed.", qPrintable(file_name));
        return ;
    }

    QTextStream text_stream(&file);

    QDomDocument xml_dom_doc;
    QString strHeader(QStringLiteral("version=\"1.0\" encoding=\"utf-8\"" ));
    xml_dom_doc.appendChild(xml_dom_doc.createProcessingInstruction(QStringLiteral("xml"), strHeader) );
    QDomElement root_node = xml_dom_doc.createElement(QStringLiteral("readback-backup"));
    root_node.setAttribute(QStringLiteral("version"), QStringLiteral("1.0"));
    xml_dom_doc.appendChild(root_node);

    saveGeneral(xml_dom_doc, root_node);
    saveRBInfo(xml_dom_doc, root_node);

    xml_dom_doc.save(text_stream, 4);
}

ReadbackPartitionInfoList ReadbackBackup::getRBManualList() const
{
    return m_rb_manual_list;
}

ReadbackPartitionInfoList ReadbackBackup::getRBAutoList() const
{
    return m_rb_auto_list;
}

bool ReadbackBackup::loadGeneral(const QDomElement &root_elememnt)
{
    QDomElement general_node = root_elememnt.firstChildElement(QStringLiteral("general"));
    if (general_node.isNull()) {
        LOG("general node not exists.");
        return false;
    }
    QDomElement chip_name_node = general_node.firstChildElement(QStringLiteral("chip-name"));
    if (chip_name_node.isNull()) {
        LOG("chip-name node not exists.");
        return false;
    }
    m_chip_name = chip_name_node.text().trimmed();
    QDomElement storage_type_node = general_node.firstChildElement(QStringLiteral("storage-type"));
    if (storage_type_node.isNull()) {
        LOG("storage-type node not exists.");
        return false;
    }
    QString storage_type = storage_type_node.text().trimmed();
    if (storage_type.isEmpty()) {
        LOG("storage-type node has no value.");
        return false;
    }
    m_storage_type = Utils::getHWStorageTypeByName(storage_type.toStdString());
    return true;
}

bool ReadbackBackup::loadRBInfo(const QDomElement &root_element)
{
    QDomElement readback_node = root_element.firstChildElement(QStringLiteral("readback"));
    if (readback_node.isNull()) {
        LOG("readback node not exists");
        return false;
    }

    auto load_rb_list = [&](const QString &elem_title, ReadbackPartitionInfoList &rb_list)->bool {
            QDomElement rb_list_node = readback_node.firstChildElement(elem_title);
            if (rb_list_node.isNull()) {
                LOG("readback-list node not exists.");
                return false;
            }
            QDomElement readback_item = rb_list_node.firstChildElement(QStringLiteral("readback-item"));
            while (!readback_item.isNull()) {
                ReadbackPartitionInfo rb_info;
                rb_info.load(readback_item);
                rb_list.append(rb_info);
                readback_item = readback_item.nextSiblingElement();
            }
            return true;
    };

    if (!load_rb_list(QStringLiteral("manual-readback-list"), m_rb_manual_list)) {
        return false;
    }
    return load_rb_list(QStringLiteral("auto-readback-list"), m_rb_auto_list);
}

void ReadbackBackup::saveGeneral(QDomDocument &xml_doc, QDomElement &root_element) const
{
    QDomElement general_node = xml_doc.createElement(QStringLiteral("general"));
    root_element.appendChild(general_node);

    QDomElement chip_name_node = xml_doc.createElement(QStringLiteral("chip-name"));
    general_node.appendChild(chip_name_node);
    QDomText text_node = xml_doc.createTextNode(m_chip_name);
    chip_name_node.appendChild(text_node);

    QDomElement storage_type_node = xml_doc.createElement(QStringLiteral("storage-type"));
    general_node.appendChild(storage_type_node);
    text_node = xml_doc.createTextNode(Utils::getStorageNameByType(m_storage_type).c_str());
    storage_type_node.appendChild(text_node);
}

void ReadbackBackup::saveRBInfo(QDomDocument &xml_doc, QDomElement &root_elemenet) const
{
    QDomElement readback_node = xml_doc.createElement(QStringLiteral("readback"));
    root_elemenet.appendChild(readback_node);

    auto save_rb_list = [&](const QString &elem_title, const ReadbackPartitionInfoList &rb_list)->void {
        QDomElement rb_list_node = xml_doc.createElement(elem_title);
        readback_node.appendChild(rb_list_node);
        foreach(auto rb_item, rb_list) {
            rb_item.save(xml_doc, rb_list_node);
        }
    };

    save_rb_list(QStringLiteral("manual-readback-list"), m_rb_manual_list);
    save_rb_list(QStringLiteral("auto-readback-list"), m_rb_auto_list);
}
