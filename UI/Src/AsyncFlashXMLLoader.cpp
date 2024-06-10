#include "AsyncFlashXMLLoader.h"
#include <exception>
#include "../../XML/Parser/FlashXMLParser.h"
#include "../../XML/Parser/ScatterXMLParser.h"
#include "../../Err/Exception.h"
#include "../../Logger/Log.h"
#include "../../Inc/CFlashToolAPI.h"
#include "../../Cmd/MainWindowCallBack.h"

#define PARSE_FAILED_ERROR_CODE -1

AsyncFlashXMLLoader::AsyncFlashXMLLoader(const std::shared_ptr<MainWindowCallBack> mainwindow_callback,
                                         const QString &flash_xml_file, QObject *parent/* = nullptr*/):
    QThread(parent),
    m_flash_xml_file(flash_xml_file),
    m_mainwindow_callback(mainwindow_callback)
{

}

void AsyncFlashXMLLoader::run()
{
    int error_code = 0;
    QString error_msg;
    bool parse_success = true;
    try {
        emit signal_start_timer();
        m_flash_xml_data_loader.setFlash_xml_file(m_flash_xml_file);
        loadFlashXmlFile();
        imageChksumVerify();
    } catch (const BaseException &e) {
        error_code = e.err_code();
        error_msg = e.err_msg();
        QString context_error_msg = e.err_msg() + QStringLiteral("\n") + e.context();
        LOG("parse flash xml caught BaseException: %s.", qPrintable(context_error_msg));
        parse_success = false;
    } catch (const std::exception &e) {
        error_code = std::numeric_limits<int>::max();
        error_msg = e.what();
        LOG("parse flash xml caught std::exception: %s.", qPrintable(error_msg));
        parse_success = false;
    } catch (...) {
        error_code = std::numeric_limits<int>::max();
        error_msg = tr("parse flash xml unknown error ocurred.");
        LOG(qPrintable(error_msg));
        parse_success = false;
    }

    if (parse_success) {
        emit signal_parse_flash_xml_successed(m_flash_xml_data_loader);
    } else {
        if (error_code == STATUS_IMAGE_CHK_CANCELLED) {
            emit signal_parse_flash_xml_cancelled();
        } else {
            emit signal_parse_flash_xml_failed(error_msg, m_flash_xml_file);
        }
    }
}

void AsyncFlashXMLLoader::loadFlashXmlFile()
{
    FlashXMLParser flash_xml_parser;
    bool load_success = flash_xml_parser.parseXMLFile(m_flash_xml_file);
    if (!load_success) {
        THROW_APP_EXCEPTION(PARSE_FAILED_ERROR_CODE, flash_xml_parser.get_error_msg())
    }

    m_flash_xml_data_loader.setContact_name(flash_xml_parser.getContact());
    QString scatter_file_name = flash_xml_parser.getScatterFileName();
    m_flash_xml_data_loader.setScatter_file_name(scatter_file_name);
    loadScatterXmlFile(scatter_file_name);
}

void AsyncFlashXMLLoader::loadScatterXmlFile(const QString &scatter_file_name)
{
    ScatterXMLParser scatter_xml_parser;
    bool load_scatter_success = scatter_xml_parser.parseXMLFile(scatter_file_name);
    if (!load_scatter_success) {
        THROW_APP_EXCEPTION(PARSE_FAILED_ERROR_CODE, scatter_xml_parser.get_error_msg())
    }

    m_flash_xml_data_loader.setChip_name(scatter_xml_parser.get_chip_name());
    m_flash_xml_data_loader.setStorage_type(scatter_xml_parser.get_storage_type());
    m_flash_xml_data_loader.setSkip_pt_operate(scatter_xml_parser.isSkipPTOperation());
    m_flash_xml_data_loader.setDl_partition_list(scatter_xml_parser.get_scatter_partitions());
}

void AsyncFlashXMLLoader::imageChksumVerify()
{
    struct callback_transfer_t cb_chksum;
    m_mainwindow_callback->setupChksumCallbackStruct(&cb_chksum);
    QString scatter_file = m_flash_xml_data_loader.getScatter_file_name();
    int result = CFlashToolAPI::cflashtool_images_checker_verify(scatter_file, &cb_chksum);
    if (result == STATUS_IMAGE_CHK_SUCCEEDED || result == STATUS_IMAGE_CHK_XML_NOT_FOUND) {
        return ;
    }
    // deal with checksum error, included user cancel operation.
    if (result == STATUS_IMAGE_CHK_CANCELLED) {
        THROW_APP_EXCEPTION(result, QStringLiteral("load flash xml file cancelled."))
    } else {
        QString error_msg("image checksum mismatch");
        THROW_APP_EXCEPTION(result, error_msg)
    }
}
