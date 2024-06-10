#ifndef ASYNCFLASHXMLLOADER_H
#define ASYNCFLASHXMLLOADER_H

#include <QObject>
#include <QThread>
#include <memory>
#include "FlashXMLDataLoader.h"

class MainWindowCallBack;

class AsyncFlashXMLLoader : public QThread
{
    Q_OBJECT

public:
    AsyncFlashXMLLoader(const std::shared_ptr<MainWindowCallBack> mainwindow_callback,
                        const QString &flash_xml_file, QObject *parent = nullptr);

    virtual void run() override;

signals:
    void signal_parse_flash_xml_successed(FlashXMLDataLoader flash_xml_data_loader);
    void signal_parse_flash_xml_failed(const QString &error_msg, const QString &flash_xml_file);
    void signal_parse_flash_xml_cancelled();
    void signal_start_timer();

private:
    void loadFlashXmlFile();
    void loadScatterXmlFile(const QString &scatter_file_name);
    void imageChksumVerify();

private:
    QString m_flash_xml_file;
    FlashXMLDataLoader m_flash_xml_data_loader;
    std::shared_ptr<MainWindowCallBack> m_mainwindow_callback;
};

#endif // ASYNCFLASHXMLLOADER_H
