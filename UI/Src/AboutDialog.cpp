#include "AboutDialog.h"
#include "ui_AboutDialog.h"
#include "MainWindow.h"

#include "../../Utility/version.h"
#include "../../Host/Inc/RuntimeMemory.h"
#include "../../Inc/CFlashToolConst.h"
#include "MultiLanguageMapping.h"

AboutDialog::AboutDialog(MainWindow *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog),
    m_main_window(parent)
{
    ui->setupUi(this);

    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    UpdateUI();
}

AboutDialog::~AboutDialog()
{
    if(ui)
    {
        delete ui;
        ui = nullptr;
    }
}


void AboutDialog::UpdateVersionInfo()
{
    //tool version info
    ui->label_title->setText(ToolInfo::ToolName().c_str());

    if(ToolInfo::IsCustomerVer())
        ui->label_comment->setText(GET_TRANSLATE_STRING(IDS_STRING_CUSTOM_RELEASE) + ToolInfo::Comments().c_str());
    else
        ui->label_comment->setText(GET_TRANSLATE_STRING(IDS_STRING_INTERNEL_RELEASE) + ToolInfo::Comments().c_str());

    ui->label_build_date->setText(GET_TRANSLATE_STRING(IDS_STRING_BUILD) + ToolInfo::BuildInfo().c_str());

    /*
    //da info
    DA_INFO da_info;
    if(main_window->main_controller()->GetDAInfo(&da_info))
    {
        ui->label_DA->setText(QString().sprintf("DA_v%s",da_info.version));
        ui->label_DA_Build_Date->setText(LoadQString(main_window->GetLanguageTag(), IDS_STRING_BUILD_DATE) + QString().sprintf("%s", da_info.last_modified_date));
    }
    else
    {
        ui->label_DA->setText("");
        ui->label_DA_Build_Date->setText("");
    }

    //library version info
    const char *p_version      = NULL;
    const char *p_release_type = NULL;
    const char *p_build_date   = NULL;

    Brom_GetDLLInfo(&p_version,&p_release_type,&p_build_date,NULL);
    ui->label_Brom->setText(QString().sprintf("libflashtool.so v%s",p_version));
    ui->label_Brom_Release_Type->setText(p_release_type);
    ui->label_Brom_Build_Date->setText(LoadQString(main_window->GetLanguageTag(), IDS_STRING_BUILD_DATE) + QString().sprintf("%s", p_build_date));
    */
    if (m_main_window->isFlashXMLLoaded()) {
        //update da & dll info
    }
}

void AboutDialog::showDADLLInfo(bool show)
{
    if (show) {
        ui->groupBox_DA_and_DLL_Info->setTitle(GET_TRANSLATE_STRING(IDS_STRING_DA_INFO));
    }
    ui->groupBox_DA_and_DLL_Info->setVisible(show);
}

void AboutDialog::UpdateUI()
{
    UpdateVersionInfo();
    showDADLLInfo(m_main_window->isFlashXMLLoaded());

    setWindowTitle(GET_TRANSLATE_STRING(IDS_STRING_ABOUT_TOOL));
    ui->label_copyright->setText(GET_TRANSLATE_STRING(IDS_STRING_COPY_RIGHT));
    ui->pushButton_Ok->setText(STRING_OK);
}
