#ifndef COMMANDSETTING_H
#define COMMANDSETTING_H

#include <list>
#include <functional>
#include <QMap>
#include <QObject>
#include "IConsoleModeXMLInterface.h"
#include "../Cmd/CmdWrapper.h"
#include "../Conn/Connection.h"
#include "CommandLineArguments.h"
#include "Utility/PartitionInfo.h"

namespace ConsoleMode {

class CommandSetting: public QObject, public IConsoleModeXMLInterface
{
    Q_OBJECT

    typedef std::function<cmd_wrapper_ptr ()> cmd_creator_function;
public:
    CommandSetting();
    CommandSetting(bool reboot_to_atm, const QString &flash_xml_file);
    CommandSetting(const std::shared_ptr<CommandLineArguments> &args);

    virtual void loadXML(const QDomElement &xml_node) Q_DECL_OVERRIDE;
    virtual void saveXML(QDomDocument &xml_doc, QDomElement &xml_node) const Q_DECL_OVERRIDE;

    void addCmdWrapper(std::shared_ptr<CmdWrapper> cmd_wrapper);
    void addCmdWrapper(CmdWrapperSPList cmd_wrapper_list);
    inline const CmdWrapperSPList &getCmdWrapperList() const { return m_cmd_wrapper_list; }

    static QStringList getSupportedInputCmds();
    bool hasRSCCmdSetting() const;
    bool hasDownloadCmd() const;
    void removeRSCSetting();
    QString getRSCFileName() const;

private slots:
    void slot_after_rpmb_cmd(int *p_result, const std::shared_ptr<Connection> conn,
            const std::shared_ptr<ICmdXMLGenerator> iCmdXMLGenerator);
    void slot_after_dram_repair_cmd(int *p_result, const std::shared_ptr<Connection> conn,
                                    const std::shared_ptr<ICmdXMLGenerator> iCmdXMLGenerator);
    void slot_after_check_storage_life_cycle_cmd(int *p_result, const std::shared_ptr<Connection> conn,
                                                 const std::shared_ptr<ICmdXMLGenerator> iCmdXMLGenerator);
    void slot_after_get_DL_img_feedback_cmd(int *p_result, const std::shared_ptr<Connection> conn,
                                                 const std::shared_ptr<ICmdXMLGenerator> iCmdXMLGenerator);

private:
    void registerCmdCreator();
    void createCmdSettingFromArgs();
    bool isDownloadCmd(const QString &cmd_name) const;
    bool isEnableWatchDogCmd(const QString &cmd_name) const;
    cmd_wrapper_ptr createEraseFlashCmdWrapper() const;
    cmd_wrapper_ptr createRPMBCmdWrapper();
    cmd_wrapper_ptr createMetaBootModeCmdWrapper() const;
    cmd_wrapper_ptr createATMModeCmdWrapper() const;
    cmd_wrapper_ptr createRSCCmdWrapper() const;
    cmd_wrapper_ptr createWDTCmdWrapper() const;
    cmd_wrapper_ptr createFlashAllExCmdWrapper() const;
    cmd_wrapper_ptr createFlashUpdateExCmdWrapper() const;
    cmd_wrapper_ptr createWriteFlashCmdWrapper() const;
    cmd_wrapper_ptr createWritePartitionsCmdWrapper() const;
    cmd_wrapper_ptr createReadFlashCmdWrapper() const;
    cmd_wrapper_ptr createReadPartitionCmdWrapper() const;
    cmd_wrapper_ptr createDramRepairCmdWrapper();
    cmd_wrapper_ptr createCheckStorageLifeCycleCmdWrapper() const;
    cmd_wrapper_ptr createWriteEfuseCmdWrapper() const;
    cmd_wrapper_ptr createReadEfuseCmdWrapper() const;
    cmd_wrapper_ptr createGetDLImgFeedbackCmdWrapper() const;
    void createCmdWrapper(const QString &cmd_name, const QDomElement &command_node = QDomElement());
    QString getScatterFile() const;
    DownloadPartitionInfoList getDLListFromScatter(const QString &scatter_file) const;
    void addRebootToATMModeCmd();
    bool needCheckDLImageChksum() const;
    QString getLoadChksumFile() const;

private:
    CmdWrapperSPList m_cmd_wrapper_list;
    const std::shared_ptr<CommandLineArguments> m_cmd_args;
    QMap<QString, cmd_creator_function> m_cmd_creator_map;
    static QMap<QString, QStringList> m_cmd_split_map;
    QString m_flash_xml_file;
    bool m_has_rsc_cmd;
    bool m_has_download_cmd;
    bool m_reboot_to_atm;
};

}

#endif // COMMANDSETTING_H
