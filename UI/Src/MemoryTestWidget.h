#ifndef MEMORYTESTWIDGET_H
#define MEMORYTESTWIDGET_H

#include "TabWidgetBase.h"

namespace Ui {
class MemoryTestWidget;
}

class MainWindow;
class CmdWrapper;
class Connection;
class ICmdXMLGenerator;

class MemoryTestWidget : public TabWidgetBase
{
    Q_OBJECT

public:
    explicit MemoryTestWidget(MainWindow *mainwindow, QTabWidget *parent);
    ~MemoryTestWidget();

    DECLARE_TABWIDGET_VFUNCS()

    void cbMemoryTestProgress(void *_this, unsigned int progress,
                          unsigned long long data_xferd, const QString &info);
    void ShowDRAMRepairInfo(bool isShow);

private:
    void setUIInitState();
    void clearUIText();
    QPair<bool, QString> validBeforeMemoryTest() const;
    QPair<bool, QString> validBeforeDRAMRepair() const;
    QPair<bool, QString> validBeforeDRAMInterfaceDiagnose() const;
    void updateUI(const QString &msg, QColor color = Qt::black);
    std::shared_ptr<CmdWrapper> createCmdWrapper();
    std::shared_ptr<CmdWrapper> createDramRepairCmdWrapper();
    std::shared_ptr<CmdWrapper> createDRAMInterfaceDiagnoseCmdWrapper();
    void doDramRepairOperation();
    void doMemoryTestOperation();
    void doDRAMInterfaceDiagnoseOperation();

private slots:
    void on_toolButton_start_clicked();
    void on_pushButton_dram_repair_clicked();
    void on_toolButton_stop_clicked();
    void on_radioButton_dramtest_manual_clicked(bool checked);
    void on_radioButton_dramtest_auto_clicked(bool checked);
    void on_DRAM_FLIP_TEST_clicked(bool checked = false);
    void on_groupBox_dram_repair_clicked(bool checked = false);
    void on_chk_dram_interface_diagnose_clicked(bool checked = false);

    void slot_before_memory_test_cmd(int *p_result, const std::shared_ptr<Connection> conn);
    void slot_after_dram_repair_cmd(int *p_result, const std::shared_ptr<Connection> conn,
                                    const std::shared_ptr<ICmdXMLGenerator> iCmdXMLGenerator);
    void slot_after_dram_interface_diagnose_cmd(int *p_result, const std::shared_ptr<Connection> conn,
                                                const std::shared_ptr<ICmdXMLGenerator> iCmdXMLGenerator);

private:
    Ui::MemoryTestWidget *ui;

    MainWindow *m_mainwindow;
    static int m_current_tested_count;
};

#endif // MEMORYTESTWIDGET_H
