#ifndef BROMADAPTER_H
#define BROMADAPTER_H

#include "TabWidgetBase.h"

namespace Ui {
class BromAdapterWidget;
}

class MainWindow;
class CmdWrapper;

class BromAdapterWidget : public TabWidgetBase
{
    Q_OBJECT

public:
    explicit BromAdapterWidget(MainWindow *mainwindow, QTabWidget *parent);
    ~BromAdapterWidget();

    DECLARE_TABWIDGET_VFUNCS()

    void cbBromAdapterProgress(void *_this, unsigned int progress,
                          unsigned long long data_xferd, const QString &info);

private:
    std::shared_ptr<CmdWrapper> createCmdWrapper();
    QPair<bool, QString> validDownloadFile() const;
    QPair<bool, QString> validBeforeOperation() const;

private slots:
        void on_toolButton_download_clicked();

        void on_toolButton_stop_clicked();

        void on_pushButton_data_clicked();

        void lockUI(bool lock);

private:
    Ui::BromAdapterWidget *ui;

    MainWindow *m_mainwindow;
};

#endif // BROMADAPTER_H
