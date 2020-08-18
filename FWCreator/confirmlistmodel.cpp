#include "confirmlistmodel.h"


QString getFilePath(const FileItem * fileItem){
    FileItem * item=(FileItem *)fileItem;
    QString file_path=item->getFileName();
    item=item->getParent();
    while(item->getParent()!=NULL){
        file_path=item->getFileName().append("/").append(file_path);
        item=item->getParent();
    }

    return file_path;
}


ConfirmListModel::ConfirmListModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    binary_ico.addFile(":/icons/binary.png");
    script_ico.addFile(":/icons/script.png");
    config_ico.addFile(":/icons/config.png");
    log_ico.addFile(":/icons/logfile.png");
    unknown_ico.addFile(":/icons/unknown.png");
}
int ConfirmListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return fileList.size();
}
int ConfirmListModel::columnCount(const QModelIndex &parent) const{
    Q_UNUSED(parent);
    return COLUMN;
}
QVariant ConfirmListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal){
        return QVariant();
    }
     switch (section){
        case NAME_COLUMN:
         if(role == Qt::DisplayRole){
             return tr("File Names");
         }
        break;
        default:
         return QVariant();
     }
     return QVariant();
}
QVariant ConfirmListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= fileList.size() || index.row() < 0){return QVariant();}

    const ConfirmFile * record = &fileList.at(index.row());

    switch(index.column()){
        case NAME_COLUMN:
            if(role == Qt::DisplayRole){
                return record->filePath;
            }
            if(role==Qt::DecorationRole){
                switch(record->fileItem->getFileType())
                {
                    case FileItem::IS_BIN:
                        return binary_ico;
                    case FileItem::IS_SCRIPT:
                        return script_ico;
                     case FileItem::IS_CFG:
                        return config_ico;
                     case FileItem::IS_LOG:
                        return log_ico;
                     default:
                        return unknown_ico;
                  }
            }
        break;
        default:
        return QVariant();
    }

    return QVariant();
}

const QList<ConfirmFile> & ConfirmListModel::getConfirmList()
{
    return fileList;
}

void ConfirmListModel::clear(){
    fileList.clear();
    reset();
}

#ifdef HAVE_QT5
void ConfirmListModel::reset()
{
    beginResetModel();
    endResetModel();
}
#endif

void ConfirmListModel::addSelectFile(const QModelIndex & index)
{
//    qDebug() <<__func__ << index;

    FileItem * fileItem = static_cast<FileItem*>(index.internalPointer());


    if(fileItem->getFileType()==FileItem::IS_DIR){//диретории в список не добавлются
        return;
    }

    qint64 id=index.internalId();

    QList<ConfirmFile>::Iterator iter;
    if((iter=isRecordExist(id))==fileList.end()){
       if(fileItem->getConfirmFlag()==FileItem::THIS_SELECT){
            ConfirmFile confirmFile;
            confirmFile.fileID=id;
            confirmFile.filePath=getFilePath(fileItem);
            confirmFile.fileItem=fileItem;
            fileList.push_back(confirmFile);
            reset();
       }
    }else{
        if(fileItem->getConfirmFlag()!=FileItem::THIS_SELECT){
            fileList.erase(iter);
            reset();
        }
    }
    emit countConfirmList(fileList.count());
}

void ConfirmListModel::removeSelected(const QModelIndexList &removeList)
{
    for(int i=0;i<removeList.size();i++){
        FileItem * item =fileList.at(removeList.at(i).row()-i).fileItem;
        QModelIndex index=createIndex(item->row(),0,item);
        emit changeCheckState(index,QVariant(Qt::Unchecked));//удаление происходит автоматически
    }
    reset();
}

void ConfirmListModel::removeSelectedFromHdd(const QModelIndexList &removeList)
{
    for(int i=0;i<removeList.size();i++){
        FileItem * item =fileList.at(removeList.at(i).row()-i).fileItem;
        QModelIndex index=createIndex(item->row(),0,item);
        emit sendRemoveSelectFromHdd(index);    //удаление конфига из файловой системы
    }
    reset();
}

QList<ConfirmFile>::Iterator ConfirmListModel::isRecordExist(qint64 id)
{
    QList<ConfirmFile>::Iterator iter= fileList.begin();

    while(iter!=fileList.end()){

        if(iter->fileID==id){
            return iter;
        }
        iter++;
    }
    return iter;
}
