#ifndef HISTORYCOMBOBOX_H
#define HISTORYCOMBOBOX_H

#include <QComboBox>
#include <QStringList>

class HistoryComboBox : public QComboBox
{
    Q_OBJECT

public:
    explicit HistoryComboBox(QComboBox *parent = nullptr);
    ~HistoryComboBox();

public:
    void removeAll(const QString &text);
    void appendItem(const QString &text);

protected:
    void loadHistory(const QString &section_name, const QString &item_name);
    void saveHistory(const QString &section_name, const QString &item_name) const;

private:
    void on_comboBox_activated(int index);

private:
    QStringList m_history;
};

class FlashXMLHistoryComboBox: public HistoryComboBox
{
public:
    explicit FlashXMLHistoryComboBox(QComboBox *parent = nullptr);
    ~FlashXMLHistoryComboBox();
};

class AuthFileHistoryComboBox: public HistoryComboBox
{
public:
    explicit AuthFileHistoryComboBox(QComboBox *parent = nullptr);
    ~AuthFileHistoryComboBox();
};

class CertFileHistoryComboBox: public HistoryComboBox
{
public:
    explicit CertFileHistoryComboBox(QComboBox *parent = nullptr);
    ~CertFileHistoryComboBox();
};

#endif // HISTORYCOMBOBOX_H
