#include "filesystemmodel.h"
#include <stdio.h>
#include <stdint.h>
#include <QDir>

#ifdef _MSC_VER
#define __func__ __FUNCTION__
#endif
/*методы работы по файловое системе*/
int getFileType(QFileInfo info){//получить тип файла
    FILE * file;
    const char* file_name=info.filePath().toLocal8Bit().data();
    file=fopen(file_name,"r");
    if(file==NULL){
        return FileItem::IS_UNKN;
    }
    char * file_header=new char[4];
    fread (file_header,1,4,file);
    fclose(file);
    uint32_t  bin_header=*((uint32_t *)file_header);
    uint16_t  script_header=*((uint16_t *)file_header);
    delete [] file_header;



    if(bin_header==0x464C457F){//идентификатор запускаемого файла
        return FileItem::IS_BIN;
    }
    if(script_header==0x2123){//идентификатор скрипта "#!"
        return FileItem::IS_SCRIPT;
    }
    QString ext=info.suffix();
    if(ext=="cfg")
    {
        return FileItem::IS_CFG;
    }
    if(ext=="log"){
        return FileItem::IS_LOG;
    }
    return FileItem::IS_UNKN;
}


DirInfo  FileSystemModel::recursiveAddDir(QString dir_path, FileItem * parent)//рекурсивно заполнить модель
{
    QDir dir(dir_path);
   // uint64_t folder_size=0;//размер папки
   // uint64_t total_files=0;//количество файлов впапке
    DirInfo dir_info;
    DirInfo prev_dir;


    FileItem * new_item;
    QFileInfoList list = dir.entryInfoList();
    for (int i=0;i<list.count();i++){
         QFileInfo info = list[i];
         if(info.isDir() && !info.isSymLink() && info.fileName()!="." && info.fileName()!="..")
         {
             new_item=new FileItem(info.fileName(),parent,FileItem::IS_DIR,info.lastModified(),0);
             parent->addChild(new_item);
             //folder_size+=recursiveAddDir(info.filePath(),new_item);
             prev_dir=recursiveAddDir(info.filePath(),new_item);
             dir_info.folder_size+=prev_dir.folder_size;
             dir_info.total_elements+=prev_dir.total_elements;
             dir_info.total_elements++;

         }else if(info.isFile() && !info.isSymLink()){
            new_item = new FileItem(info.fileName(),parent,getFileType(info),info.lastModified(),info.size());
            new_item->setFilePath(info.filePath());
//            new_item = new FileItem(info, parent, getFileType(info));

            dir_info.folder_size+=info.size();
            dir_info.total_elements++;
            parent->addChild(new_item);
            new_item->setFlagFile(true);

            const QString filename = new_item->getFileName();
            if (isMarked(filename)) {
                new_item->setConfirmFlag(FileItem::THIS_SELECT);
                QModelIndex index=this->createIndex(new_item->row(), 0, new_item);
                setConfirmToBranch(index, FileItem::THIS_SELECT);
                setConfirmToParent(index, FileItem::THIS_SELECT);
            }
            if (isWarning(filename)) {
                emit warningFile(filename);
                new_item->setWarning(true);
            }
         }
    }

    parent->setFileSize(dir_info.folder_size);
    parent->setTotalChild(dir_info.total_elements);
    return dir_info;

}

//помечаем для подтверждения
bool FileSystemModel::isMarked(const QString filename) const
{
    QStringList ls = QStringList() << "geometry.cfg" << "calibr.cfg";
    foreach (QString text, ls) {
        if (text == filename)
            return true;
    }
    return false;
}

//сообщаем о наличии
bool FileSystemModel::isWarning(const QString filename) const
{
    QStringList ls = QStringList() << "calibr.cfg";
    foreach (QString text, ls) {
        if (text == filename)
            return true;
    }
    return false;
}


int getFileTypeFromZip(ZipFileItem *zip_item){

    switch(zip_item->getType()){
        case ZipFileItem::IS_DIR:
            return FileItem::IS_DIR;
        break;
        case ZipFileItem::IS_BIN:
            return FileItem::IS_BIN;
        break;
        case ZipFileItem::IS_SCRIPT:
            return FileItem::IS_SCRIPT;
        break;
        case ZipFileItem::IS_CFG:
            return FileItem::IS_CFG;
        break;
        case ZipFileItem::IS_LOG:
            return FileItem::IS_LOG;
        break;
    }
    return FileItem::IS_UNKN;
}

QDateTime getDateTimeFromZip(ZipFileItem *zip_item){
    ZipDate zip_date=zip_item->getZipDate();
    QDate date(zip_date.year+1980,zip_date.month,zip_date.day);
    QTime time(zip_date.hour,zip_date.minute,zip_date.second);
    return QDateTime(date,time);
}

void fillFileSystem(FileItem * fs_item, ZipFileItem * zip_fs_item){
    fs_item->setTotalChild(zip_fs_item->getTotalChildCount());
    fs_item->setFileSize(zip_fs_item->getUncompressSize());
    for(unsigned int i=0;i<zip_fs_item->getChildCount();i++){
        ZipFileItem * zip_item=zip_fs_item->getChild(i);
        FileItem * item=new FileItem(QString(zip_item->getName().c_str()),
                                     fs_item,
                                     getFileTypeFromZip(zip_item),
                                     getDateTimeFromZip(zip_item),
                                     zip_item->getUncompressSize());
        fs_item->addChild(item);
        fillFileSystem(item,zip_item);
    }
}

FileSystemModel::FileSystemModel()
{
    rootItem=new FileItem("root");
    folder_ico.addFile(":/icons/folder.png");
    binary_ico.addFile(":/icons/binary.png");
    script_ico.addFile(":/icons/script.png");
    config_ico.addFile(":/icons/config.png");
    log_ico.addFile(":/icons/logfile.png");
    unknown_ico.addFile(":/icons/unknown.png");
}
QVariant FileSystemModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();
    FileItem *item = static_cast<FileItem*>(index.internalPointer());

    switch(index.column()){
        case FileSystemModel::COUNT_COLUMN:
           if(role == Qt::DisplayRole){
               if (item->isFile())
                    return QString("%1").arg(1);
               else
                   return QString("%1").arg(item->getTotalChild());
           }
        break;
        case FileSystemModel::NAME_COLUMN:
           if(role == Qt::DisplayRole){
               return item->getFileName();
           }
           if(role == Qt::CheckStateRole){
               //return (item->getConfirmFlag() ? return QVariant(Qt::Checked); : QVariant(Qt::Unchecked));
               switch(item->getConfirmFlag()){
                    case FileItem::CHILD_SELECT:
                        return QVariant(Qt::PartiallyChecked);
                    break;
                    case FileItem::THIS_SELECT:
                        return QVariant(Qt::Checked);
                    break;
                    case FileItem::THIS_UNSELECT:
                        return QVariant(Qt::Unchecked);
                    break;
                    default:
                        return QVariant();
               }
           }
           if(role==Qt::DecorationRole){
               switch(item->getFileType()){
                   case FileItem::IS_DIR:
                       return folder_ico;
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

        case FileSystemModel::SIZE_COLUMN:
            if(role == Qt::DisplayRole){
                uint64_t tera=1099511627776ULL;//1 терабайт данных
                uint64_t giga=1024*1024*1024;
                uint64_t mega=1024*1024;
                uint64_t kilo=1024;
                uint64_t file_size=item->getFileSize();

                if(file_size > tera){//определяем какой размер файла
                    return QString::number((((double)file_size) / tera),'f',2).append(tr(" Tb"));
                }else if(file_size > giga){
                    return QString::number((((double)file_size) / giga),'f',2).append(tr(" Gb"));
                }else if(file_size > mega){
                    return QString::number((((double)file_size) / mega),'f',2).append(tr(" Mb"));
                }else if(file_size > kilo){
                    return QString::number((((double)file_size) / kilo),'f',2).append(tr(" Kb"));
                }
                    return QString::number(file_size);
            }
            if(role==Qt::TextAlignmentRole){
                return QVariant(Qt::AlignRight | Qt::AlignVCenter);
            }
        break;
        case FileSystemModel::DATE_COLUMN:
            if(role == Qt::DisplayRole){
                return item->getTimeInfo();
            }
            if(role==Qt::TextAlignmentRole){
                return QVariant(Qt::AlignRight | Qt::AlignVCenter);
            }

        break;

    }

   return QVariant();
}

bool FileSystemModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(role == Qt::CheckStateRole && index.column() == 0) {
       Qt::CheckState state= static_cast<Qt::CheckState>(value.toInt());
       switch(state){
            case Qt::Unchecked:
                setConfirmToBranch(index,FileItem::THIS_UNSELECT);
                setConfirmToParent(index,FileItem::THIS_UNSELECT);
                return true;
            break;
            case Qt::Checked:
                setConfirmToBranch(index,FileItem::THIS_SELECT);
                setConfirmToParent(index,FileItem::THIS_SELECT);
                return true;
            case Qt::PartiallyChecked:
                //setConfirmToBranch(index,FileItem::CHILD_SELECT);
                return true;
            break;
       }
    }
    return false;
}
 Qt::ItemFlags FileSystemModel::flags( const QModelIndex & index ) const
 {
        return QAbstractItemModel::flags( index ) | Qt::ItemIsUserCheckable;
 }

QVariant FileSystemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal){
        return QVariant();
    }

        switch (section){
            case FileSystemModel::NAME_COLUMN:
                if(role == Qt::DisplayRole){
                    return tr("File Names");
                }
            break;
            case FileSystemModel::COUNT_COLUMN:
                if(role == Qt::DisplayRole){
                    return tr("Count");
                }
            break;
            case FileSystemModel::SIZE_COLUMN:
                if(role == Qt::DisplayRole){
                    return tr("Size");
                }
                if(role==Qt::TextAlignmentRole){
                    return QVariant(Qt::AlignCenter | Qt::AlignVCenter);
                }
            break;
            case FileSystemModel::DATE_COLUMN:
                if(role == Qt::DisplayRole){
                    return tr("Date modified");
                }
                if(role==Qt::TextAlignmentRole){
                    return QVariant(Qt::AlignCenter | Qt::AlignVCenter);
                }
            break;
            default:
                return QVariant();
        }
    return QVariant();
}



QModelIndex  FileSystemModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    FileItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<FileItem*>(parent.internalPointer());

    FileItem *childItem = parentItem->getChild(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}
QModelIndex FileSystemModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    FileItem *childItem = static_cast<FileItem*>(index.internalPointer());
    FileItem *parentItem = childItem->getParent();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int FileSystemModel::rowCount(const QModelIndex &parent) const
{
    FileItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<FileItem*>(parent.internalPointer());

    return parentItem->childCount();
}

int FileSystemModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return COLUMN;
}
/*
void FileSystemModel::addFile(QString filePath)
{
    QStringList nodes=filePath.split("/");

    FileItem * local_item=rootItem;

    bool not_found;
    for(int i=0;i<nodes.size();i++)
    {
        not_found=true;
        QString node_name=nodes[i];
        for(int j=0; j<local_item->childCount();j++)
        {
           if(!local_item->getChild(j)->getFileName().compare(node_name,Qt::CaseSensitive))//если данный элемент существует
            {
                local_item=local_item->getChild(j);
                not_found=false;
                break;
            }
        }

        if(not_found && i!= nodes.size()-1){//проверка того что существую предыдущие элементы
            emit incorrectFilePath(filePath);
            return;
        }

        if(not_found)
        {
            FileItem * new_item=new FileItem(node_name,local_item);
            local_item->addChild(new_item);
            local_item=new_item;
        }
    }
    reset();
}
*/
void FileSystemModel::addDir(QString dirPath)
{
    rootItem->clear();
    recursiveAddDir(dirPath,rootItem);
    reset();
}

#ifdef HAVE_QT5
void FileSystemModel::reset()
{
    beginResetModel();
    endResetModel();
}
#endif

void FileSystemModel::addZipStructure(ZipFileItem *zip_item){
    rootItem->clear();//очитстить все существующие элементы
    fillFileSystem(rootItem,zip_item);//заполнить файловую структуру данными из Zip
    reset();
}
void FileSystemModel::addConfirmList(QList<QString> confirm){
   QList<QString>::Iterator iter = confirm.begin();
   while(iter!=confirm.end()){
       addConfirmFile(*iter);
       iter++;
   }
}
void FileSystemModel::addConfirmFile(QString file_path)
{
    qDebug() << "addConfirmFile: " <<file_path;
    QStringList names = file_path.split("/");
    FileItem* parent_item=rootItem;
    FileItem* child_item=NULL;
    for(int i=0; i<names.size(); i++){
        if((child_item=parent_item->isItemExist(names[i]))!=NULL){
            parent_item=child_item;
        }else{
            return;
        }
    }

    if(child_item!=NULL){
        QModelIndex item_index=this->createIndex(child_item->row(),0,child_item);
        setData(item_index,Qt::Checked,Qt::CheckStateRole);
    }

}

void FileSystemModel::clear()
{
    rootItem->clear();
    reset();
}



quint64 FileSystemModel::getTotalFile()
{
    return rootItem->getTotalChild();
}

quint64 FileSystemModel::getTotalSize()
{
    return rootItem->getFileSize();
}

void FileSystemModel::setCheckState(const QModelIndex &index, const QVariant &value){
    qDebug() << __func__ << index << value;
    setData(index,value,Qt::CheckStateRole);
}

void FileSystemModel::removeSelectFromHdd(const QModelIndex &index)
{
    FileItem *node_item = static_cast<FileItem*>(index.internalPointer());

    qDebug() << __func__ << index << node_item->getFileName();

    node_item->removeFromParent(index.row());

    QFile::remove(node_item->getFilePath());

    delete node_item;

    setConfirmToParent(index, 0);
    setConfirmToBranch(index, 0);

    reset();
}

void FileSystemModel::setConfirmToParent(const QModelIndex &parent, int flag)
{
    FileItem *node_item = static_cast<FileItem*>(parent.internalPointer());
    if(node_item==NULL){
        return;
    }
    FileItem * parent_item=node_item->getParent();

    int child_flag=flag;
    for(int i=0;i<parent_item->childCount();i++){
        if(parent_item->getChild(i)->getConfirmFlag()!=child_flag){
            child_flag=parent_item->getChild(i)->getConfirmFlag();
            break;
        }
    }
    if(child_flag==flag){//если все флаги в ветке равны то на родителя выставляем этот же флаг
         parent_item->setConfirmFlag(flag);
         QModelIndex parent_index=this->parent(parent);
         emit dataChanged(parent_index, parent_index);
         setConfirmToParent(parent_index,flag);
    }else{
         parent_item->setConfirmFlag(FileItem::CHILD_SELECT);//если хоть один флаг различается то на родителе флаг сбрасывается
         QModelIndex parent_index=this->parent(parent);
         emit dataChanged(parent_index, parent_index);
         setConfirmToParent(parent_index,FileItem::CHILD_SELECT);
    }
}

void FileSystemModel::setConfirmToBranch(const QModelIndex& parent, int flag)//рекурсивное проставление checkbox на ветку
{
    FileItem *node_item = static_cast<FileItem*>(parent.internalPointer());
    node_item->setConfirmFlag(flag);

//    FileItem * parent_item=node_item->getParent();//если все child выбраны то выбирается и родитель
//    bool child_flag=flag;
//    for(int i=0;i<parent_item->childCount();i++){
//        if(parent_item->getChild(i)->getConfirmFlag()!=child_flag){
//            child_flag=parent_item->getChild(i)->getConfirmFlag();
//            break;
//        }
//    }
//    if(child_flag==flag){//если все флаги в ветке равны то на родителя выставляем этот же флаг
//        parent_item->setConfirmFlag(flag);
//        QModelIndex parent_index=this->parent(parent);
//        emit dataChanged(parent_index, parent_index);
//    }else{
//        parent_item->setConfirmFlag(false);//если хоть один флаг различается то на родителе флаг сбрасывается
//        QModelIndex parent_index=this->parent(parent);
//        emit dataChanged(parent_index, parent_index);
//    }


    //emit selectFile(parent.internalId(),node_item);
    emit selectFile(parent);

    emit dataChanged(parent, parent);
    for(int i=0; i<node_item->childCount();i++)
    {
        setConfirmToBranch(index(i,0,parent), flag);
    }
}

QString FileSystemModel::containsWarning(FileItem *item) const
{
    if (item->isWarning())
        return item->getFileName();

    for (int i=0; i<item->childCount(); i++) {
        QString res = containsWarning(item->getChild(i));
        if (!res.isEmpty())
            return res;
    }
    return QString("");
}

QString FileSystemModel::checkWarnings()
{
    return containsWarning(rootItem);
}



