#include "HistoryComboBox.h"

HistoryComboBox::HistoryComboBox(QComboBox *parent) :
    QComboBox(parent)
{

}

HistoryComboBox::~HistoryComboBox()
{

}

void HistoryComboBox::removeAll(const QString &text)
{
    Q_UNUSED(text)
}

void HistoryComboBox::appendItem(const QString &text)
{
    Q_UNUSED(text)
}

void HistoryComboBox::loadHistory(const QString &section_name, const QString &item_name)
{
    Q_UNUSED(section_name)
    Q_UNUSED(item_name)
}

void HistoryComboBox::saveHistory(const QString &section_name, const QString &item_name) const
{
    Q_UNUSED(section_name)
    Q_UNUSED(item_name)
}

void HistoryComboBox::on_comboBox_activated(int index)
{
    Q_UNUSED(index)
}

FlashXMLHistoryComboBox::FlashXMLHistoryComboBox(QComboBox *parent):
    HistoryComboBox(parent)
{
    this->loadHistory("RecentOpenFile", "flashxmlHistory");
}

FlashXMLHistoryComboBox::~FlashXMLHistoryComboBox()
{
    this->saveHistory("RecentOpenFile", "flashxmlHistory");
}

AuthFileHistoryComboBox::AuthFileHistoryComboBox(QComboBox *parent):
    HistoryComboBox(parent)
{
    this->loadHistory("RecentOpenFile", "authHistory");
}

AuthFileHistoryComboBox::~AuthFileHistoryComboBox()
{
    this->saveHistory("RecentOpenFile", "authHistory");
}

CertFileHistoryComboBox::CertFileHistoryComboBox(QComboBox *parent):
    HistoryComboBox(parent)
{
    this->loadHistory("RecentOpenFile", "certHistory");
}

CertFileHistoryComboBox::~CertFileHistoryComboBox()
{
    this->saveHistory("RecentOpenFile", "certHistory");
}
