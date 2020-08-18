#include "ipaddressmodel.h"
#include "CommandLineParams.h"
#include <string.h>

IPAddressModel::IPAddressModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    QString t=CommandLineParser::instance().getValDef("-ip", "192.168.0.120");

    ip_list.push_back(QString (t));
    ip_reg_exp=new QRegExp( "\\b(([01]?\\d?\\d|2[0-4]\\d|25[0-5])\\.){3}([01]?\\d?\\d|2[0-4]\\d|25[0-5])\\b");

}




int IPAddressModel::rowCount(const QModelIndex &index) const{
    Q_UNUSED(index);
    return ip_list.size();
}

int IPAddressModel::columnCount(const QModelIndex &index) const{
    Q_UNUSED(index);
    return TOTAL_COLUMN;
}

QVariant IPAddressModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= ip_list.size() || index.row() < 0){return QVariant();}
    if(role==Qt::DisplayRole){
        return ip_list[index.row()];
    }
    return QVariant();
}

bool IPAddressModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!index.isValid() || index.row()<0  || index.row()>=ip_list.size() ){
        return false;
    }
    if(role==Qt::EditRole && index.column()==0)
    {
        ip_list.replace(index.row(),value.toString());
        return true;
    }
    return false;
}

Qt::ItemFlags IPAddressModel::flags(const QModelIndex &index) const{
    if(!index.isValid()){return Qt::ItemIsEnabled;}
    if(index.column()==0){
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    }
    return Qt::ItemIsEnabled;
}

QStringList IPAddressModel::getIPList(){
    return ip_list;
}
bool IPAddressModel::isValid()
{
    if(ip_list.empty()){//проверка на то что список IP не пустой
        return false;
    }
    for(int i=0;i<ip_list.size();i++){
        QString ip=ip_list.at(i);
        if(ip.isEmpty() || !ip_reg_exp->exactMatch(ip)){
            return false;
        }
    }
    return true;
}

void IPAddressModel::setIPAdress(QStringList ip_list){
    this->ip_list=ip_list;
    reset();
}

#ifdef HAVE_QT5
void IPAddressModel::reset()
{
    beginResetModel();
    endResetModel();
}
#endif

void IPAddressModel::removeSelected(const QModelIndexList &removeList)
{
    for(int i=0; i<removeList.size();i++){
        int rem_index=removeList.at(i).row()-i;
        ip_list.removeAt(rem_index);
    }
    reset();
}
void IPAddressModel::addIPField()
{
    ip_list.push_back(QString());
    reset();
}


