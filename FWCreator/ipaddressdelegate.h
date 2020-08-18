#ifndef IPADDRESSDELEGATE_H
#define IPADDRESSDELEGATE_H

#include <QItemDelegate>
#include <QRegExp>
#include <QRegExpValidator>

class IPAddressDelegate : public QItemDelegate
{
public:
    IPAddressDelegate(QObject *parent = 0);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
private:
    QRegExp * ip_reg_exp;
    QRegExpValidator * reg_exp_valid;

};

#endif // IPADDRESSDELEGATE_H
