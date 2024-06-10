#include "CommandSetting.h"

#include <QSet>
#include <QDir>

#include "ConsoleModeRunner.h"
#include "../XML/CmdGenerator/EraseFlashCmdXML.h"
#include "../XML/CmdGenerator/EMMCControlCmdXML.h"
#include "../XML/CmdGenerator/SetBootModeCmdXML.h"
#include "../XML/CmdGenerator/SetRSCCmdXML.h"
#include "../XML/CmdGenerator/RebootCmdXML.h"
#include "../XML/CmdGenerator/FlashAllExCmdXML.h"
#include "../XML/CmdGenerator/FlashUpdateExCmdXML.h"
#include "../XML/CmdGenerator/WritePartitionsCmdXML.h"
#include "../XML/CmdGenerator/ReadFlashCmdXML.h"
#include "../XML/CmdGenerator/ReadPartitionCmdXML.h"
#include "../XML/CmdGenerator/WriteFlashCmdXML.h"
#include "../XML/CmdGenerator/DebugDRAMRepairCmdXML.h"
#include "../XML/CmdGenerator/ReadEfuseCmdXML.h"
#include "../XML/CmdGenerator/WriteEfuseCmdXML.h"
#include "../XML/CmdGenerator/GetDLImageFeedbackCmdXML.h"
#include "../XML/Parser/FlashXMLParser.h"
#include "../XML/Parser/ScatterXMLParser.h"
#include "../Err/Exception.h"
#include "../Err/CFlashToolErrorCode.h"
#include "../Utility/FileUtils.h"
#include "../Logger/Log.h"
#include <cassert>

namespace ConsoleMode
{

static const QString RSC_FILE_NAME = "rsc.xml";
#define CHKSUM_FILE_NAME QStringLiteral("scatter_checksum.xml")

// convert user-interface commands to internal commands.
QMap<QString, QStringList> CommandSetting::m_cmd_split_map = {
    {FORMAT_ALL_CMD_NAME, {ERASE_FLASH_CMD_NAME_VAL}},
    {DL_ONLY_CMD_NAME, {WRITE_PARTITIONS_CMD_NAME_VAL}},
    {FMT_ALL_DL_CMD_NAME, {ERASE_FLASH_CMD_NAME_VAL, FLASH_ALL_EX_CMD_NAME_VAL}},
    {FIRMWARE_UPGRADE_CMD_NAME, {FLASH_UPDATE_EX_CMD_NAME_VAL}},
    {DRAM_REPAIR_CMD_NAME, {DEBUG_DRAM_REPAIR_CMD_NAME_VAL}},
    {WRITE_EFUSE_CMD_NAME, {WRITE_EFUSE_CMD_NAME_VAL}},
    {READ_EFUSE_CMD_NAME, {READ_EFUSE_CMD_NAME_VAL}}
};

CommandSetting::CommandSetting():
    m_cmd_args(nullptr),
    m_has_rsc_cmd(false),
    m_has_download_cmd(false),
    m_reboot_to_atm(false)
{
    registerCmdCreator();
}

CommandSetting::CommandSetting(bool reboot_to_atm, const QString &flash_xml_file):
    m_cmd_args(nullptr),
    m_flash_xml_file(flash_xml_file),
    m_has_rsc_cmd(false),
    m_has_download_cmd(false),
    m_reboot_to_atm(reboot_to_atm)
{
    registerCmdCreator();
}

CommandSetting::CommandSetting(const std::shared_ptr<CommandLineArguments> &args):
    m_cmd_args(args),
    m_flash_xml_file(args->getFlashXMLFilename()),
    m_has_rsc_cmd(false),
    m_has_download_cmd(false),
    m_reboot_to_atm(args->reboot_to_atm())
{
    registerCmdCreator();
    createCmdSettingFromArgs();
}

void CommandSetting::loadXML(const QDomElement &xml_node)
{
    QDomElement commands_node = xml_node.firstChildElement(QStringLiteral("commands"));
    if (commands_node.isNull()) {
        THROW_APP_EXCEPTION(E_ERROR, QStringLiteral("commands node not exists."))
    }
    QDomElement command_node = commands_node.firstChildElement();
    while (!command_node.isNull()) {
        QString command_name = command_node.tagName().trimmed();
        command_name = Add_CMD_PREFIX(command_name); // add "CMD:" prefix to cmd name.
        if (this->isDownloadCmd(command_name)) {
            m_has_download_cmd = true;
        }
        if (command_name == SET_RSC_CMD_NAME_VAL) {
            m_has_rsc_cmd = true;
        }
        //add ATM cmd if before reboot cmd, which need to be the last one.
        if (command_name == REBOOT_CMD_NAME_VAL) {
            this->addRebootToATMModeCmd();
        }
        createCmdWrapper(command_name, command_node);
        command_node = command_node.nextSiblingElement();
    }
}

void CommandSetting::saveXML(QDomDocument &xml_doc, QDomElement &xml_node) const
{
    QDomElement commands_node = xml_doc.createElement(QStringLiteral("commands"));
    xml_node.appendChild(commands_node);
    for(auto cmd_wrapper: m_cmd_wrapper_list) {
       cmd_wrapper->saveXML(xml_doc, commands_node);
    }
}

void CommandSetting::addCmdWrapper(std::shared_ptr<CmdWrapper> cmd_wrapper)
{
    m_cmd_wrapper_list.push_back(cmd_wrapper);
}

void CommandSetting::addCmdWrapper(CmdWrapperSPList cmd_wrapper_list)
{
    foreach(auto cmd_wrapper, cmd_wrapper_list) {
        m_cmd_wrapper_list.push_back(cmd_wrapper);
    }
}

QStringList CommandSetting::getSupportedInputCmds()
{
    return m_cmd_split_map.keys();
}

bool CommandSetting::hasRSCCmdSetting() const
{
    return m_has_rsc_cmd;
}

bool CommandSetting::hasDownloadCmd() const
{
    return m_has_download_cmd;
}

void CommandSetting::removeRSCSetting()
{
    foreach (auto cmdWrapper, m_cmd_wrapper_list) {
        std::shared_ptr<SetRSCCmdXML> rsc_cmd_xml =
                std::dynamic_pointer_cast<SetRSCCmdXML>(cmdWrapper->get_cmd_xml_generator());
        if (rsc_cmd_xml) {
            m_cmd_wrapper_list.remove(cmdWrapper);
            LOGI("remove RSC command from console mode command list.");
            break;
        }
    }
}

void CommandSetting::slot_after_rpmb_cmd(int *p_result, const std::shared_ptr<Connection> conn, const std::shared_ptr<ICmdXMLGenerator> iCmdXMLGenerator)
{
    *p_result = ConsoleModeCallBack::cb_after_rpmb_cmd(conn, iCmdXMLGenerator);
}

void CommandSetting::slot_after_dram_repair_cmd(int *p_result, const std::shared_ptr<Connection> conn, const std::shared_ptr<ICmdXMLGenerator> iCmdXMLGenerator)
{
    *p_result = ConsoleModeCallBack::cb_after_dram_repair_cmd(conn, iCmdXMLGenerator);
}

void CommandSetting::slot_after_check_storage_life_cycle_cmd(int *p_result, const std::shared_ptr<Connection> conn, const std::shared_ptr<ICmdXMLGenerator> iCmdXMLGenerator)
{
    *p_result = ConsoleModeCallBack::cb_after_check_storage_life_cycle_cmd(conn, iCmdXMLGenerator);
}

void CommandSetting::slot_after_get_DL_img_feedback_cmd(int *p_result, const std::shared_ptr<Connection> conn, const std::shared_ptr<ICmdXMLGenerator> iCmdXMLGenerator)
{
    *p_result = ICmdCallBack::cb_after_get_dl_img_feedback_cmd(conn, iCmdXMLGenerator, getLoadChksumFile());
}

void CommandSetting::registerCmdCreator()
{
    m_cmd_creator_map = {
        {ERASE_FLASH_CMD_NAME_VAL, std::bind(&CommandSetting::createEraseFlashCmdWrapper, this)},
        {Add_CMD_PREFIX(GET_RPMB_KEY_STATUS), std::bind(&CommandSetting::createRPMBCmdWrapper, this)},
        {Add_CMD_PREFIX(LIFE_CYCLE_CHECK), std::bind(&CommandSetting::createCheckStorageLifeCycleCmdWrapper, this)},
        {SET_BOOT_MODE_CMD_NAME_VAL, std::bind(&CommandSetting::createMetaBootModeCmdWrapper, this)},
        {SET_RSC_CMD_NAME_VAL, std::bind(&CommandSetting::createRSCCmdWrapper, this)},
        {REBOOT_CMD_NAME_VAL, std::bind(&CommandSetting::createWDTCmdWrapper, this)},
        {FLASH_ALL_EX_CMD_NAME_VAL, std::bind(&CommandSetting::createFlashAllExCmdWrapper, this)},
        {FLASH_UPDATE_EX_CMD_NAME_VAL, std::bind(&CommandSetting::createFlashUpdateExCmdWrapper, this)},
        {WRITE_PARTITIONS_CMD_NAME_VAL, std::bind(&CommandSetting::createWritePartitionsCmdWrapper, this)},
        {READ_FLASH_CMD_NAME_VAL, std::bind(&CommandSetting::createReadFlashCmdWrapper, this)},
        {READ_PARTITION_CMD_NAME_VAL, std::bind(&CommandSetting::createReadPartitionCmdWrapper, this)},
        {WRITE_FLASH_CMD_NAME_VAL, std::bind(&CommandSetting::createWriteFlashCmdWrapper, this)},
        {DEBUG_DRAM_REPAIR_CMD_NAME_VAL, std::bind(&CommandSetting::createDramRepairCmdWrapper, this)},
        {WRITE_EFUSE_CMD_NAME_VAL, std::bind(&CommandSetting::createWriteEfuseCmdWrapper, this)},
        {READ_EFUSE_CMD_NAME_VAL, std::bind(&CommandSetting::createReadEfuseCmdWrapper, this)},
        {GET_DOWNLOADED_IMAGE_FEEDBACK_CMD_NAME_VAL, std::bind(&CommandSetting::createGetDLImgFeedbackCmdWrapper, this)}
    };
}

void CommandSetting::createCmdSettingFromArgs()
{
    assert(m_cmd_args);
    QString cmd_name = m_cmd_args->getCommandName();
    if (cmd_name.isEmpty()) {
        return;
    }
    // MUST add the Check Storage Life Cycle Cmd as the first one if needed. To avoid forget it, so write here.
    if (m_cmd_args->needStorageLifeCycleCheck() &&
            GeneralSetting::getConnStageByCommand(cmd_name) == CONN_END_STAGE::SECOND_DA_STAGE) {
        addCmdWrapper(this->createCheckStorageLifeCycleCmdWrapper());
    }

    bool needWDTCmd = false;
    QStringList split_cmd_names = m_cmd_split_map.value(cmd_name);
    foreach(auto split_cmd_name, split_cmd_names) {
        createCmdWrapper(split_cmd_name);
        if (this->isDownloadCmd(split_cmd_name)) {
            m_has_download_cmd = true;
        }
        if (this->isEnableWatchDogCmd(split_cmd_name)) {
            needWDTCmd = true;
        }
    }

    if (this->needCheckDLImageChksum()) {
        addCmdWrapper(this->createGetDLImgFeedbackCmdWrapper());
    }

    if (!needWDTCmd) {
        needWDTCmd = m_cmd_args->needReboot();
    }

    // rsc command
    if (!m_cmd_args->getRSCProjName().isEmpty() && m_has_download_cmd)
    {
        addCmdWrapper(createRSCCmdWrapper());
        m_has_rsc_cmd = true;
    }

    this->addRebootToATMModeCmd();

    std::shared_ptr<CmdWrapper> wdt_cmd = this->createWDTCmdWrapper();
    std::shared_ptr<RebootCmdXML> reboot_wdt_cmd = std::dynamic_pointer_cast<RebootCmdXML>(wdt_cmd->get_cmd_xml_generator());
    reboot_wdt_cmd->setRebootType(needWDTCmd ? REBOOT_TYPE::IMMEDIATE : REBOOT_TYPE::DISCONNECT);
    addCmdWrapper(wdt_cmd);
}

bool CommandSetting::isDownloadCmd(const QString &cmd_name) const
{
    static QSet<QString> cmd_sets;
    //cmd_sets << "firmware-upgrade" << "download" << "format-download";
    cmd_sets << FLASH_UPDATE_EX_CMD_NAME_VAL << WRITE_PARTITIONS_CMD_NAME_VAL << FLASH_ALL_EX_CMD_NAME_VAL;
    return cmd_sets.contains(cmd_name);
}

bool CommandSetting::isEnableWatchDogCmd(const QString &cmd_name) const
{
    if (isDownloadCmd(cmd_name)) {
        return true;
    }
    if (cmd_name == DEBUG_DRAM_REPAIR_CMD_NAME_VAL) {
        return true;
    }
    return false;
}

cmd_wrapper_ptr CommandSetting::createEraseFlashCmdWrapper() const
{
    cmd_wrapper_ptr cmd_wrapper = std::make_shared<CmdWrapper>(std::make_shared<EraseFlashCmdXML>());
    cmd_wrapper->setOperationProgress(ConsoleModeRunner::cbFormatProgress);
    return  cmd_wrapper;
}

cmd_wrapper_ptr CommandSetting::createRPMBCmdWrapper()
{
    std::shared_ptr<GetRPMBKeyStatusCmdXML> getRPMBStatusCmdXml = std::make_shared<GetRPMBKeyStatusCmdXML>();
    getRPMBStatusCmdXml->setTargetFileType(FILE_TYPE::MEMORY_FILE);
    cmd_wrapper_ptr cmd_wrapper = std::make_shared<CmdWrapper>(getRPMBStatusCmdXml);
    qRegisterMetaType<std::shared_ptr<Connection>>("std::shared_ptr<Connection>");
    qRegisterMetaType<std::shared_ptr<ICmdXMLGenerator>>("std::shared_ptr<ICmdXMLGenerator>");
    QObject::connect(cmd_wrapper.get(), &CmdWrapper::signal_after_cmd,
                     this, &CommandSetting::slot_after_rpmb_cmd);
    return  cmd_wrapper;
}

cmd_wrapper_ptr CommandSetting::createMetaBootModeCmdWrapper() const
{
    return std::make_shared<CmdWrapper>(std::make_shared<MetaBootModeCmdXML>());
}

cmd_wrapper_ptr CommandSetting::createATMModeCmdWrapper() const
{
    return std::make_shared<CmdWrapper>(std::make_shared<AndroidTestModeCmdXMl>());
}

cmd_wrapper_ptr CommandSetting::createRSCCmdWrapper() const
{
    std::shared_ptr<SetRSCCmdXML> setRSCCmdXml = std::make_shared<SetRSCCmdXML>();
    if (m_cmd_args && m_cmd_args->getCurrentMode() == CommandLineArguments::ConsoleModeCurrentMode::CmdArgumentMode) {
        setRSCCmdXml->setKey(m_cmd_args->getRSCProjName());
        setRSCCmdXml->setFileName(getRSCFileName());
    }
    cmd_wrapper_ptr cmd_wrapper = std::make_shared<CmdWrapper>(setRSCCmdXml);
    return  cmd_wrapper;
}

cmd_wrapper_ptr CommandSetting::createWDTCmdWrapper() const
{
    return std::make_shared<CmdWrapper>(std::make_shared<RebootCmdXML>());
}

cmd_wrapper_ptr CommandSetting::createFlashAllExCmdWrapper() const
{
    std::shared_ptr<FlashAllExCmdXML> flashAllExCmdXML = std::make_shared<FlashAllExCmdXML>();
    if (m_cmd_args && m_cmd_args->getCurrentMode() == CommandLineArguments::ConsoleModeCurrentMode::CmdArgumentMode) {
        QString scatter_file = getScatterFile();
        flashAllExCmdXML->setFileName(scatter_file);
        flashAllExCmdXML->setDLPartitionList(getDLListFromScatter(scatter_file));
    }
    cmd_wrapper_ptr cmd_wrapper = std::make_shared<CmdWrapper>(flashAllExCmdXML);
    cmd_wrapper->setOperationProgress(ConsoleModeRunner::cbDownloadProgress);
    return  cmd_wrapper;
}

cmd_wrapper_ptr CommandSetting::createFlashUpdateExCmdWrapper() const
{
    std::shared_ptr<FlashUpdateExCmdXML> flashUpdateExCmdXML = std::make_shared<FlashUpdateExCmdXML>();
    if (m_cmd_args && m_cmd_args->getCurrentMode() == CommandLineArguments::ConsoleModeCurrentMode::CmdArgumentMode) {
        QString scatter_file = getScatterFile();
        flashUpdateExCmdXML->setBackupFolder(FileUtils::FWBackupFolder());
        flashUpdateExCmdXML->setFileName(scatter_file);
        flashUpdateExCmdXML->setDLPartitionList(getDLListFromScatter(scatter_file));
    }
    cmd_wrapper_ptr cmd_wrapper = std::make_shared<CmdWrapper>(flashUpdateExCmdXML);
    cmd_wrapper->setOperationProgress(ConsoleModeRunner::cbDownloadProgress);
    return  cmd_wrapper;
}

cmd_wrapper_ptr CommandSetting::createWritePartitionsCmdWrapper() const
{
    std::shared_ptr<WritePartitionsCmdXML> writePartitionsCmdXML = std::make_shared<WritePartitionsCmdXML>();
    if (m_cmd_args && m_cmd_args->getCurrentMode() == CommandLineArguments::ConsoleModeCurrentMode::CmdArgumentMode) {
        QString scatter_file = getScatterFile();
        writePartitionsCmdXML->setFileName(scatter_file);
        writePartitionsCmdXML->setDLPartitionList(getDLListFromScatter(scatter_file));
    }
    cmd_wrapper_ptr cmd_wrapper = std::make_shared<CmdWrapper>(writePartitionsCmdXML);
    cmd_wrapper->setOperationProgress(ConsoleModeRunner::cbDownloadProgress);
    return  cmd_wrapper;
}

cmd_wrapper_ptr CommandSetting::createReadFlashCmdWrapper() const
{
    cmd_wrapper_ptr cmd_wrapper = std::make_shared<CmdWrapper>(std::make_shared<ReadFlashCmdXML>());
    cmd_wrapper->setOperationProgress(ConsoleModeRunner::cbReadbackProgress);
    return  cmd_wrapper;
}

cmd_wrapper_ptr CommandSetting::createReadPartitionCmdWrapper() const
{
    cmd_wrapper_ptr cmd_wrapper = std::make_shared<CmdWrapper>(std::make_shared<ReadPartitionCmdXML>());
    cmd_wrapper->setOperationProgress(ConsoleModeRunner::cbReadbackProgress);
    return  cmd_wrapper;
}

cmd_wrapper_ptr CommandSetting::createDramRepairCmdWrapper()
{
    std::shared_ptr<DebugDRAMRepairCmdXML> dramRepairCmdXML = std::make_shared<DebugDRAMRepairCmdXML>();
    if (m_cmd_args && m_cmd_args->getCurrentMode() == CommandLineArguments::ConsoleModeCurrentMode::CmdArgumentMode) {
        dramRepairCmdXML->setParam_file(m_cmd_args->getFile());
        dramRepairCmdXML->setFileType(FILE_TYPE::MEMORY_FILE);
    }
    std::shared_ptr<CmdWrapper> cmd_wrapper = std::make_shared<CmdWrapper>(dramRepairCmdXML);
    QObject::connect(cmd_wrapper.get(), &CmdWrapper::signal_after_cmd, this, &CommandSetting::slot_after_dram_repair_cmd);
    return  cmd_wrapper;
}

cmd_wrapper_ptr CommandSetting::createCheckStorageLifeCycleCmdWrapper() const
{
    std::shared_ptr<LifeCycleStatusCmdXML> checkStorageLifeCycleCmd = std::make_shared<LifeCycleStatusCmdXML>();
    checkStorageLifeCycleCmd->setTargetFileType(FILE_TYPE::MEMORY_FILE);
    std::shared_ptr<CmdWrapper> cmd_wrapper = std::make_shared<CmdWrapper>(checkStorageLifeCycleCmd);
    qRegisterMetaType<std::shared_ptr<Connection>>("std::shared_ptr<Connection>");
    qRegisterMetaType<std::shared_ptr<ICmdXMLGenerator>>("std::shared_ptr<ICmdXMLGenerator>");
    QObject::connect(cmd_wrapper.get(), &CmdWrapper::signal_after_cmd,
                     this, &CommandSetting::slot_after_check_storage_life_cycle_cmd);
    return  cmd_wrapper;
}

cmd_wrapper_ptr CommandSetting::createWriteEfuseCmdWrapper() const
{
    std::shared_ptr<WriteEfuseCmdXML> writeEfuseCmdXML = std::make_shared<WriteEfuseCmdXML>();
    if (m_cmd_args && m_cmd_args->getCurrentMode() == CommandLineArguments::ConsoleModeCurrentMode::CmdArgumentMode) {
        writeEfuseCmdXML->setFileName(m_cmd_args->getFile());
    }
    return std::make_shared<CmdWrapper>(writeEfuseCmdXML);
}

cmd_wrapper_ptr CommandSetting::createReadEfuseCmdWrapper() const
{
    std::shared_ptr<ReadEfuseCmdXML> readEfuseCmdXML = std::make_shared<ReadEfuseCmdXML>();
    if (m_cmd_args && m_cmd_args->getCurrentMode() == CommandLineArguments::ConsoleModeCurrentMode::CmdArgumentMode) {
        readEfuseCmdXML->setFileName(m_cmd_args->getFile());
    }
    return std::make_shared<CmdWrapper>(readEfuseCmdXML);
}

cmd_wrapper_ptr CommandSetting::createGetDLImgFeedbackCmdWrapper() const
{
    std::shared_ptr<GetDLImageFeedbackCmdXML> dl_feedback_cmd = std::make_shared<GetDLImageFeedbackCmdXML>();
    dl_feedback_cmd->setFileType(FILE_TYPE::MEMORY_FILE);
    std::shared_ptr<CmdWrapper> cmd_wrapper = std::make_shared<CmdWrapper>(dl_feedback_cmd);
    qRegisterMetaType<std::shared_ptr<Connection>>("std::shared_ptr<Connection>");
    qRegisterMetaType<std::shared_ptr<ICmdXMLGenerator>>("std::shared_ptr<ICmdXMLGenerator>");
    QObject::connect(cmd_wrapper.get(), &CmdWrapper::signal_after_cmd,
                     this, &CommandSetting::slot_after_get_DL_img_feedback_cmd);
    return  cmd_wrapper;
}

cmd_wrapper_ptr CommandSetting::createWriteFlashCmdWrapper() const
{
    cmd_wrapper_ptr cmd_wrapper = std::make_shared<CmdWrapper>(std::make_shared<WriteFlashCmdXML>());
    cmd_wrapper->setOperationProgress(ConsoleModeRunner::cbWriteMemoryProgress);
    return  cmd_wrapper;
}

void CommandSetting::createCmdWrapper(const QString &cmd_name,
                                      const QDomElement &command_node /*= QDomElement()*/)
{
    if (m_cmd_creator_map.contains(cmd_name)) {
        std::shared_ptr<CmdWrapper> cmd_wrapper = m_cmd_creator_map.value(cmd_name)();
        if (!command_node.isNull()) {
            cmd_wrapper->loadXML(command_node);
        }
        addCmdWrapper(cmd_wrapper);
    } else {
        THROW_APP_EXCEPTION(CFT_NOT_SUPPORTED_CONSOLE_MODE_CMD, QStringLiteral("%1 command is NOT supported in console mode.").arg(cmd_name))
    }
}

QString CommandSetting::getScatterFile() const
{
    if (m_flash_xml_file.isEmpty()) {
        return QString();
    }
    FlashXMLParser flash_xml_parser;
    bool load_success = flash_xml_parser.parseXMLFile(m_flash_xml_file);
    if (load_success) {
        return flash_xml_parser.getScatterFileName();
    } else {
        LOGE(qPrintable(flash_xml_parser.get_error_msg()));
        THROW_APP_EXCEPTION(E_ERROR, qPrintable(flash_xml_parser.get_error_msg()))
    }
}

QString CommandSetting::getRSCFileName() const
{
    QString scatter_file = this->getScatterFile();
    if (scatter_file.isEmpty()) {
        return QString();
    }

    QString rsc_dir = (QFileInfo(scatter_file)).absolutePath();
    QString rsc_file_path = rsc_dir + QDir::separator().toLatin1() + RSC_FILE_NAME;
    rsc_file_path = QDir::toNativeSeparators(rsc_file_path);
    return rsc_file_path;
}

DownloadPartitionInfoList CommandSetting::getDLListFromScatter(const QString &scatter_file) const
{
    ScatterXMLParser scatter_xml_parser;
    bool load_scatter_success = scatter_xml_parser.parseXMLFile(scatter_file);
    if (!load_scatter_success) {
        LOGE(qPrintable(scatter_xml_parser.get_error_msg()));
        THROW_APP_EXCEPTION(E_ERROR, qPrintable(scatter_xml_parser.get_error_msg()))
    }
    return scatter_xml_parser.get_scatter_partitions();
}

void CommandSetting::addRebootToATMModeCmd()
{
    // Android Test Mode
    if (this->hasDownloadCmd() && m_reboot_to_atm) {
        addCmdWrapper(this->createATMModeCmdWrapper());
    }
}

bool CommandSetting::needCheckDLImageChksum() const
{
    // return true only when USB Checksum enabled and scatter_checksum.xml exists in load.
    // but in console mode argument mode, the USB Checksum always disabled, so return false.
    // this function is a way to prevent bug when USB Checksum enabled in future.
    return false;
}

QString CommandSetting::getLoadChksumFile() const
{
    QFileInfo file_info(this->getScatterFile());
    return QFileInfo(file_info.absoluteDir(), CHKSUM_FILE_NAME).absoluteFilePath();
}

}
