#ifndef IPADDRESSMODEL_H
#define IPADDRESSMODEL_H

#include <QAbstractItemModel>
#include <QStringList>
#include <QModelIndexList>

class IPAddressModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    IPAddressModel(QObject *parent = 0);
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);
    Qt::ItemFlags flags ( const QModelIndex & index ) const ;
    QStringList getIPList();
    bool isValid();
public slots:
    void setIPAdress(QStringList ip_list);
    void removeSelected(const QModelIndexList & removeList);
    void addIPField();
private slots:
#ifdef HAVE_QT5
    void reset();
#endif
private:
    static const int TOTAL_COLUMN=1;
    QStringList ip_list;//список доступных адресов IP
    QRegExp* ip_reg_exp;

};

#endif // IPADDRESSMODEL_H
