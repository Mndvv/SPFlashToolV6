#include "CheckHeader.h"

CheckHeader::CheckHeader(Qt::Orientation orientation, QWidget *parent) :
    QHeaderView(orientation, parent),
    ischecked_(false)
{
    QFont font = this->font();
    font.setBold(true);
    this->setFont(font);
    setStretchLastSection(true);
    setSectionResizeMode(QHeaderView::Fixed);

    setStyleSheet("QHeaderView::section {background-color:rgba(170,170,255, 50%);}");
}

void CheckHeader::SetChecked(bool checked)
{
    if(ischecked_ != checked)
    {
        ischecked_ = checked;
        updateSection(0);
    }
}

void CheckHeader::paintSection(QPainter *painter, const QRect &rect,
                               int logicalIndex) const
{
    painter->save();
    QHeaderView::paintSection(painter, rect, logicalIndex);
    painter->restore();

    if(logicalIndex == 0)
    {
        QStyleOptionButton option;

        if(isEnabled()) {
            option.state |= QStyle::State_Enabled;
            option.state |= QStyle::State_Selected;
        }

        option.state |= ischecked_ ? QStyle::State_On : QStyle::State_Off;
        option.rect = CheckBoxRect(rect);

        style()->drawControl(QStyle::CE_CheckBox, &option, painter);
    }
}

void CheckHeader::mousePressEvent(QMouseEvent *e)
{
    if(isEnabled() && logicalIndexAt(e->pos()) == 0)
    {
        ischecked_= !ischecked_;
        updateSection(0);
        emit signal_enable_checked(ischecked_);
    }
    
     QHeaderView::mousePressEvent(e);  
}

QRect CheckHeader::CheckBoxRect(const QRect &sourceRect) const
{
    QStyleOptionButton option;
    QRect rect = style()->subElementRect(QStyle::SE_CheckBoxIndicator, &option);
    QPoint checkBoxPoint(sourceRect.x() +
                            sourceRect.width() / 2 -
                            rect.width() / 2,
                            sourceRect.y() +
                            sourceRect.height() / 2 -
                            rect.height() / 2);


    return QRect(checkBoxPoint, rect.size());
}
