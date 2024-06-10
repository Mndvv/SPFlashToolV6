#ifndef WELCOMEWIDGET_H
#define WELCOMEWIDGET_H

#include "TabWidgetBase.h"

namespace Ui
{
class WelcomeWidget;
}

class MainWindow;

class WelcomeWidget : public TabWidgetBase
{
    Q_OBJECT

public:

    WelcomeWidget(MainWindow *mainwindow, QTabWidget *parent);
    ~WelcomeWidget();

    DECLARE_TABWIDGET_VFUNCS()

private:
    Ui::WelcomeWidget *ui;
    MainWindow *m_main_window;
};

#endif // WELCOMEWIDGET_H
