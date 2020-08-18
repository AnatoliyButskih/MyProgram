#include "ipaddressdelegate.h"
#include <QLineEdit>


IPAddressDelegate::IPAddressDelegate(QObject *parent):
    QItemDelegate(parent)
{
    ip_reg_exp=new QRegExp( "\\b(([01]?\\d?\\d|2[0-4]\\d|25[0-5])\\.){3}([01]?\\d?\\d|2[0-4]\\d|25[0-5])\\b");
    reg_exp_valid=new QRegExpValidator(*ip_reg_exp,this);
}
QWidget * IPAddressDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(index.column()==0){
        QLineEdit * line_edit=new QLineEdit(parent);
        line_edit->setValidator(reg_exp_valid);
        return line_edit;
    }
    return QItemDelegate::createEditor(parent,option,index);
}

void IPAddressDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const{
    if(index.column()==0){
        QLineEdit * line_edit = static_cast<QLineEdit*>(editor);
        line_edit->setText(index.model()->data(index, Qt::DisplayRole).toString());
    }
}

void IPAddressDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const{
    if(index.column()==0){
        QLineEdit * line_edit = static_cast<QLineEdit*>(editor);
        model->setData(index,line_edit->text());
    }
}

void IPAddressDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const{
    Q_UNUSED(index);
    editor->setGeometry(option.rect);
}
