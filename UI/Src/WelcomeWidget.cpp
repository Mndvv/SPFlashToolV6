#include "WelcomeWidget.h"
#include "MainWindow.h"
#include "ui_WelcomeWidget.h"
#include "../../Host/Inc/RuntimeMemory.h"
#include "../../Inc/CFlashToolConst.h"
#include "../../Logger/Log.h"
#include "MultiLanguageMapping.h"

WelcomeWidget::WelcomeWidget(MainWindow *mainwindow, QTabWidget *parent):
    TabWidgetBase(WELCOME_WIDGET_INDEX, GET_TRANSLATE_STRING(IDS_STRING_WELCOME_PAGE_NAME), parent),
    ui(new Ui::WelcomeWidget),
    m_main_window(mainwindow)
{
    ui->setupUi(this);
}

WelcomeWidget::~WelcomeWidget()
{
    if(ui)
    {
        delete ui;
        ui = nullptr;
    }
}

void WelcomeWidget::LockOnUI()
{
}

void WelcomeWidget::DoFinished()
{

}

void WelcomeWidget::UpdateUI()
{
    ui->retranslateUi(this);
    m_main_window->refreshTabText(this, GET_TRANSLATE_STRING(IDS_STRING_WELCOME_PAGE_NAME));
}
