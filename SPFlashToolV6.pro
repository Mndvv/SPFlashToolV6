QT       += core gui xml xmlpatterns serialport concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

unix: {
    DEFINES += "_LINUX64"
    QMAKE_LFLAGS += -Wl,-rpath,.
    QMAKE_LFLAGS += -Wl,-rpath,lib
    QMAKE_LFLAGS += -no-pie
}

TRANSLATIONS += CFlashtool_zh_CN.ts CFlashtool_zh_TW.ts

INCLUDEPATH += UI/Src

# QMAKE_CXXFLAGS_RELEASE = -Od -ZI -MD
# QMAKE_LFLAGS_RELEASE = /DEBUG /INCREMENTAL:NO

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

TARGET = SPFlashToolV6

win32:LIBS += -L$$quote($$PWD/Libs) \
    -lflash \
    -limageChecker \
    -lSLA_Challenge

unix:LIBS += -L$$quote($$PWD/Libs) \
    -lflash.1.0.0 \
    -limagechecker.1.0.0 \
    -lsla_challenge

SOURCES += \
    Cmd/CmdExecutor.cpp \
    Cmd/CmdWrapper.cpp \
    Cmd/ConsoleModeCallBack.cpp \
    Cmd/ICmdCallBack.cpp \
    Cmd/MainWindowCallBack.cpp \
    Com/AsyncComPortInfo.cpp \
    Com/COMFinderWrapper.cpp \
    Conn/Connection.cpp \
    Conn/ConnectionArgs.cpp \
    ConsoleMode/CommandLineArguments.cpp \
    ConsoleMode/CommandSetting.cpp \
    ConsoleMode/CommandSettingValidator.cpp \
    ConsoleMode/Config.cpp \
    ConsoleMode/ConsoleModeEntry.cpp \
    ConsoleMode/ConsoleModeRunner.cpp \
    ConsoleMode/GeneralSetting.cpp \
    ConsoleMode/SchemaValidator.cpp \
    Err/AppException.cpp \
    Err/BaseException.cpp \
    Err/CFlashToolErrorCode.cpp \
    Inc/CFlashToolAPI.cpp \
    Logger/Logger.cpp \
    Network/NetworkIntf.cpp \
    Network/NetworkOps.cpp \
    Network/NetworkThread.cpp \
    UI/Src/AboutDialog.cpp \
    UI/Src/AsyncFlashXMLLoader.cpp \
    UI/Src/BackgroundWorker.cpp \
    UI/Src/BromAdapterWidget.cpp \
    UI/Src/BromConnSettingWidget.cpp \
    UI/Src/CheckHeader.cpp \
    UI/Src/DLCertWidget.cpp \
    UI/Src/DownloadWidget.cpp \
    UI/Src/DownloadWidgetTableModel.cpp \
    UI/Src/FFUWidget.cpp \
    UI/Src/FlashXMLDataLoader.cpp \
    UI/Src/FormatWidget.cpp \
    UI/Src/HWInfoToHTML.cpp \
    UI/Src/HistoryComboBox.cpp \
    UI/Src/MainWindow.cpp \
    UI/Src/MemoryTestWidget.cpp \
    UI/Src/MultiLanguageMapping.cpp \
    UI/Src/OkDialog.cpp \
    UI/Src/OptionDialog.cpp \
    UI/Src/ProcessingAnimation.cpp \
    UI/Src/ProcessingDialog.cpp \
    UI/Src/ReadBackWidget.cpp \
    UI/Src/ReadbackBackup.cpp \
    UI/Src/ReadbackWidgetTableDelegate.cpp \
    UI/Src/ReadbackWidgetTableModel.cpp \
    UI/Src/TabWidgetBase.cpp \
    UI/Src/WelcomeWidget.cpp \
    UI/Src/WriteMemoryWidget.cpp \
    UI/Src/main.cpp \
    Utility/FileUtils.cpp \
    Utility/IniItem.cpp \
    Utility/LogFilesClean.cpp \
    Utility/LogInfoSetting.cpp \
    Utility/OptionIniFileMgr.cpp \
    Utility/PartitionInfo.cpp \
    Utility/Utils.cpp \
    Utility/version.cpp \
    XML/CmdGenerator/BootToCmdXML.cpp \
    XML/CmdGenerator/BromReadRegCmdXML.cpp \
    XML/CmdGenerator/BromWriteRegCmdXML.cpp \
    XML/CmdGenerator/DAReadRegCmdXML.cpp \
    XML/CmdGenerator/DAWriteRegCmdXML.cpp \
    XML/CmdGenerator/DebugDRAMRepairCmdXML.cpp \
    XML/CmdGenerator/DebugUFSCmdXML.cpp \
    XML/CmdGenerator/EMMCControlCmdXML.cpp \
    XML/CmdGenerator/EraseFlashCmdXML.cpp \
    XML/CmdGenerator/ErasePartitionCmdXML.cpp \
    XML/CmdGenerator/FlashAllCmdXML.cpp \
    XML/CmdGenerator/FlashAllExCmdXML.cpp \
    XML/CmdGenerator/FlashUpdateCmdXML.cpp \
    XML/CmdGenerator/FlashUpdateExCmdXML.cpp \
    XML/CmdGenerator/GetDAInfoCmdXML.cpp \
    XML/CmdGenerator/GetDLImageFeedbackCmdXML.cpp \
    XML/CmdGenerator/GetHWInfoCmdXML.cpp \
    XML/CmdGenerator/ICmdXMLGenerator.cpp \
    XML/CmdGenerator/RamTestCmdXML.cpp \
    XML/CmdGenerator/ReadEfuseCmdXML.cpp \
    XML/CmdGenerator/ReadFlashCmdXML.cpp \
    XML/CmdGenerator/ReadPartitionCmdXML.cpp \
    XML/CmdGenerator/ReadPartitionTblCmdXML.cpp \
    XML/CmdGenerator/RebootCmdXML.cpp \
    XML/CmdGenerator/RunProgramCmdXML.cpp \
    XML/CmdGenerator/SecGetDevFWInfoCmdXML.cpp \
    XML/CmdGenerator/SecSetAllInOneSignatureCmdXML.cpp \
    XML/CmdGenerator/SecSetFlashPolicyCmdXML.cpp \
    XML/CmdGenerator/SetBootModeCmdXML.cpp \
    XML/CmdGenerator/SetHostInfoCmdXML.cpp \
    XML/CmdGenerator/SetRSCCmdXML.cpp \
    XML/CmdGenerator/SetRuntimeParaCmdXML.cpp \
    XML/CmdGenerator/WriteEfuseCmdXML.cpp \
    XML/CmdGenerator/WriteFlashCmdXML.cpp \
    XML/CmdGenerator/WritePartitionCmdXML.cpp \
    XML/CmdGenerator/WritePartitionsCmdXML.cpp \
    XML/CmdGenerator/WritePrivateCertCmdXML.cpp \
    XML/CmdGenerator/XMLFileAssistant.cpp \
    XML/Parser/ChksumParser.cpp \
    XML/Parser/FlashXMLParser.cpp \
    XML/Parser/GetHWInfoXMLParser.cpp \
    XML/Parser/HWInfoTree.cpp \
    XML/Parser/IXMLParser.cpp \
    XML/Parser/PartitionTableParser.cpp \
    XML/Parser/RSCParser.cpp \
    XML/Parser/ScatterXMLParser.cpp

# ONLY Windows related begins here
win32:SOURCES += Host/Win/CriticalSectionSentry.cpp \
    Host/Win/RuntimeMemory.cpp \
    UI/Src/AsyncUpdater.cpp \
    UI/Src/CheckUpdateDialog.cpp \
    UI/Src/ToolUpdate.cpp \
    UI/Src/UpdateDialog.cpp \
    Utility/PrintDriverVersion.cpp

# ONLY Linux related begins here
unix:SOURCES += Host/Linux/CriticalSectionSentry.cpp \
    Host/Linux/RuntimeMemory.cpp

HEADERS += \
    Cmd/CallbackTypes.h \
    Cmd/CmdExecutor.h \
    Cmd/CmdWrapper.h \
    Cmd/ConsoleModeCallBack.h \
    Cmd/ICmdCallBack.h \
    Cmd/MainWindowCallBack.h \
    Com/AsyncComPortInfo.h \
    Com/COMFinderWrapper.h \
    Conn/Connection.h \
    Conn/ConnectionArgs.h \
    ConsoleMode/CommandLineArguments.h \
    ConsoleMode/CommandSetting.h \
    ConsoleMode/CommandSettingValidator.h \
    ConsoleMode/Config.h \
    ConsoleMode/ConsoleModeEntry.h \
    ConsoleMode/ConsoleModeRunner.h \
    ConsoleMode/GeneralSetting.h \
    ConsoleMode/IConsoleModeXMLInterface.h \
    ConsoleMode/SchemaValidator.h \
    Err/AppException.h \
    Err/BaseException.h \
    Err/CFlashToolErrorCode.h \
    Err/Exception.h \
    Host/Inc/CriticalSectionSentry.h \
    Host/Inc/RuntimeMemory.h \
    Inc/CFlashToolAPI.h \
    Inc/CFlashToolConst.h \
    Inc/CFlashToolTypes.h \
    Inc/Flash.Mode.API.h \
    Inc/Flash.Mode.Struct.h \
    Inc/SLA_Challenge.h \
    Inc/error_code.h \
    Inc/images_checker.h \
    Logger/Log.h \
    Logger/Logger.h \
    Network/NetworkIntf.h \
    Network/NetworkOps.h \
    Network/NetworkThread.h \
    UI/Src/AboutDialog.h \
    UI/Src/AsyncFlashXMLLoader.h \
    UI/Src/BackgroundWorker.h \
    UI/Src/BromAdapterWidget.h \
    UI/Src/BromConnSettingWidget.h \
    UI/Src/CheckHeader.h \
    UI/Src/DLCertWidget.h \
    UI/Src/DownloadWidget.h \
    UI/Src/DownloadWidgetTableModel.h \
    UI/Src/FFUWidget.h \
    UI/Src/FlashXMLDataLoader.h \
    UI/Src/FormatWidget.h \
    UI/Src/HWInfoToHTML.h \
    UI/Src/HistoryComboBox.h \
    UI/Src/MainWindow.h \
    UI/Src/MemoryTestWidget.h \
    UI/Src/MultiLanguageMapping.h \
    UI/Src/OkDialog.h \
    UI/Src/OptionDialog.h \
    UI/Src/ProcessingAnimation.h \
    UI/Src/ProcessingDialog.h \
    UI/Src/ReadBackWidget.h \
    UI/Src/ReadbackBackup.h \
    UI/Src/ReadbackWidgetTableDelegate.h \
    UI/Src/ReadbackWidgetTableModel.h \
    UI/Src/TabWidgetBase.h \
    UI/Src/WelcomeWidget.h \
    UI/Src/WriteMemoryWidget.h \
    Utility/FileUtils.h \
    Utility/IniItem.h \
    Utility/LogFilesClean.h \
    Utility/LogInfoSetting.h \
    Utility/OptionIniFileMgr.h \
    Utility/PartitionInfo.h \
    Utility/Utils.h \
    Utility/version.h \
    Utility/version_def.h \
    XML/CmdGenerator/BootToCmdXML.h \
    XML/CmdGenerator/BromReadRegCmdXML.h \
    XML/CmdGenerator/BromWriteRegCmdXML.h \
    XML/CmdGenerator/DAReadRegCmdXML.h \
    XML/CmdGenerator/DAWriteRegCmdXML.h \
    XML/CmdGenerator/DebugDRAMRepairCmdXML.h \
    XML/CmdGenerator/DebugUFSCmdXML.h \
    XML/CmdGenerator/EMMCControlCmdXML.h \
    XML/CmdGenerator/EraseFlashCmdXML.h \
    XML/CmdGenerator/ErasePartitionCmdXML.h \
    XML/CmdGenerator/FlashAllCmdXML.h \
    XML/CmdGenerator/FlashAllExCmdXML.h \
    XML/CmdGenerator/FlashUpdateCmdXML.h \
    XML/CmdGenerator/FlashUpdateExCmdXML.h \
    XML/CmdGenerator/GetDAInfoCmdXML.h \
    XML/CmdGenerator/GetDLImageFeedbackCmdXML.h \
    XML/CmdGenerator/GetHWInfoCmdXML.h \
    XML/CmdGenerator/ICmdXMLGenerator.h \
    XML/CmdGenerator/RamTestCmdXML.h \
    XML/CmdGenerator/ReadEfuseCmdXML.h \
    XML/CmdGenerator/ReadFlashCmdXML.h \
    XML/CmdGenerator/ReadPartitionCmdXML.h \
    XML/CmdGenerator/ReadPartitionTblCmdXML.h \
    XML/CmdGenerator/RebootCmdXML.h \
    XML/CmdGenerator/RunProgramCmdXML.h \
    XML/CmdGenerator/SecGetDevFWInfoCmdXML.h \
    XML/CmdGenerator/SecSetAllInOneSignatureCmdXML.h \
    XML/CmdGenerator/SecSetFlashPolicyCmdXML.h \
    XML/CmdGenerator/SetBootModeCmdXML.h \
    XML/CmdGenerator/SetHostInfoCmdXML.h \
    XML/CmdGenerator/SetRSCCmdXML.h \
    XML/CmdGenerator/SetRuntimeParaCmdXML.h \
    XML/CmdGenerator/WriteEfuseCmdXML.h \
    XML/CmdGenerator/WriteFlashCmdXML.h \
    XML/CmdGenerator/WritePartitionCmdXML.h \
    XML/CmdGenerator/WritePartitionsCmdXML.h \
    XML/CmdGenerator/WritePrivateCertCmdXML.h \
    XML/CmdGenerator/XMLFileAssistant.h \
    XML/Parser/ChksumParser.h \
    XML/Parser/FlashXMLParser.h \
    XML/Parser/GetHWInfoXMLParser.h \
    XML/Parser/HWInfoTree.h \
    XML/Parser/IXMLParser.h \
    XML/Parser/PartitionTableParser.h \
    XML/Parser/RSCParser.h \
    XML/Parser/ScatterXMLParser.h \
    XML/XMLConst.h

win32:HEADERS += UI/Src/AsyncUpdater.h \
    UI/Src/CheckUpdateDialog.h \
    UI/Src/ToolUpdate.h \
    UI/Src/UpdateDialog.h \
    Utility/PrintDriverVersion.h

FORMS += \
    UI/Forms/AboutDialog.ui \
    UI/Forms/BromAdapterWidget.ui \
    UI/Forms/BromConnSettingWidget.ui \
    UI/Forms/DLCertWidget.ui \
    UI/Forms/DownloadWidget.ui \
    UI/Forms/FFUWidget.ui \
    UI/Forms/FormatWidget.ui \
    UI/Forms/MainWindow.ui \
    UI/Forms/MemoryTestWidget.ui \
    UI/Forms/OkDialog.ui \
    UI/Forms/OptionDialog.ui \
    UI/Forms/ProcessingDialog.ui \
    UI/Forms/ReadBackWidget.ui \
    UI/Forms/WelcomeWidget.ui \
    UI/Forms/WriteMemoryWidget.ui

win32:FORMS += UI/Forms/CheckUpdateDialog.ui \
    UI/Forms/UpdateDialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    UI/Resources/html.qrc \
    UI/Resources/icon.qrc \
    UI/Resources/translations.qrc

RC_FILE = UI/Resources/SPFlashToolV6.rc
