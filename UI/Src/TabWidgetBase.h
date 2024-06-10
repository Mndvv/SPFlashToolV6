#ifndef TABWIDGETBASE_H
#define TABWIDGETBASE_H

#include <QTableWidget>
#include <memory>

#define DECLARE_TABWIDGET_VFUNCS()    \
    virtual void LockOnUI() override;   \
    virtual void DoFinished() override;    \
    virtual void UpdateUI() override;

class TabWidgetBase: public QWidget
{
    Q_OBJECT
public:
    TabWidgetBase(int index, const QString& name, QTabWidget *parent);
    virtual ~TabWidgetBase(){}

    virtual void LockOnUI() = 0;
    virtual void DoFinished() = 0;
    virtual void UpdateUI() = 0;

    void Detach();
    void Attach();

    bool IsSelectedTab();
    void SetTabEnabled(bool enable);
protected:
    QTabWidget * tab_parent;
    const int tab_index;
    const QString tab_name;

    bool tab_visible;
};

#endif // TABWIDGETBASE_H
