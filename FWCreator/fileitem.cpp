#include "fileitem.h"
/*
FileItem::FileItem(QString fileName, FileItem * parent)
{
    is_update=false;
    this->fileName=fileName;
    parentItem=parent;
    fileType=IS_UNKN;
}
*/

FileItem::FileItem(QString fileName, FileItem * parent,int fileType, const QDateTime & fileDate, uint64_t fileSize)
{
    state=FileItem::THIS_UNSELECT;
    this->fileName=fileName;
    this->fileType=fileType;
    this->fileDate=fileDate;
    this->fileSize=fileSize;
    this->totalChild=0;
    this->warning = false;
    this->is_file = false;
    parentItem=parent;
}

FileItem::FileItem(QFileInfo info, FileItem *parent, int fileType)
{
    path = info.filePath();
    qDebug() << path;
    FileItem(info.fileName(), parent, fileType, info.lastModified(), info.size());
}

FileItem::~FileItem(){
    clear();
}

void FileItem::addChild(FileItem *child)
{
    childItems.append(child);
}
FileItem* FileItem::getChild(int row)   const
{
    if(row<childItems.size()){
        return childItems.value(row);
    }
    return NULL;
}

int FileItem::childCount() const
{
    return childItems.size();
}

int FileItem::columnCount() const
{
    return 1;
}

QVariant FileItem::data(int column) const
{
    Q_UNUSED(column);
    return fileName;
}

int FileItem::row() const
{
    if (parentItem)
        return parentItem->childItems.indexOf(const_cast<FileItem*>(this));

    return 0;
}

FileItem* FileItem::getParent()
{
    return parentItem;
}

QString FileItem::getFileName()
{
    return fileName;
}

void FileItem::setConfirmFlag(int flag)
{
    state=flag;
}

int FileItem::getConfirmFlag() const
{
    return state;
}

int FileItem::getFileType() const
{
    return fileType;
}

void FileItem::setFilePath(const QString path)
{
    this->path = path;
}

void FileItem::setFileSize(uint64_t fileSize)
{
    this->fileSize=fileSize;
}

uint64_t FileItem::getFileSize()
{
   return fileSize;
}

void FileItem::setTotalChild(uint64_t totalChild)
{
    this->totalChild=totalChild;
}

uint64_t FileItem::getTotalChild() const
{
    return totalChild;
}


const QDateTime & FileItem::getTimeInfo()
{
    return fileDate;
}

void FileItem::clear(){

    QList<FileItem*>::Iterator iter= childItems.begin();
    while(iter!=childItems.end()){
        delete *iter;
        iter++;
    }
    childItems.clear();
}

FileItem* FileItem::isItemExist(QString name){
    for(int i=0;i<childItems.size();i++){
        FileItem* child_item=getChild(i);
//        qDebug() << child_item->getFileName();
        if(child_item->getFileName()==name){
            return child_item;
        }
    }
    return NULL;
}

void FileItem::removeFromParent(const int row)
{
    this->parentItem->lessChild(row);
}
