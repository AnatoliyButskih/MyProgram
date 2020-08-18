#include "zipfileitem.h"

#include <iostream>

using namespace std;
void fillZipDate(ZipDate & date,ZipFileAttr * file_attr)
{
    date.year=(file_attr->date_mod>>9) & 0x3F;
    date.month=(file_attr->date_mod>>5) & 0xF;
    date.day=file_attr->date_mod & 0x1F;

    date.hour=(file_attr->time_mod>>11) & 0x1F;
    date.minute=(file_attr->time_mod>>5) & 0x3F;
    date.second=file_attr->time_mod & 0x1F;
}


vector<string> ZipFileItem::str_split(string & str, char delim)
{
    string::iterator iter=str.begin();
    string item;
    vector<string> elems;
    while(iter!=str.end()){
       if(*iter!=delim){
           item+=*iter;
       }else if(!item.empty()){
           elems.push_back(item);
           item.clear();
       }
       iter++;
    }
    if(!item.empty()){
        elems.push_back(item);
    }
    return elems;
}

ZipFileItem::ZipFileItem(string name, ZipFileItem * parent)
{
    this->name=name;
    this->parent=parent;
    uncompress_size=0;
    compress_size=0;
    type=IS_UNKN;
    total_child=0;
}
ZipFileItem::~ZipFileItem()
{
    clear();
}

string ZipFileItem::getName() const
{
    return name;
}

uint32_t ZipFileItem::getUncompressSize() const
{
    return uncompress_size;
}

uint32_t ZipFileItem::getCompressSize() const
{
    return compress_size;
}

uint32_t ZipFileItem::getTotalChildCount() const
{
    return total_child;
}

uint32_t ZipFileItem::getType() const
{
    return type;
}

ZipDate ZipFileItem::getZipDate() const
{
    return date;
}

size_t ZipFileItem::getChildCount() const
{
    return childs.size();
}

ZipFileItem* ZipFileItem::getParent()
{
    return parent;
}

ZipFileItem*  ZipFileItem::getChild(size_t n)
{
    if(n<childs.size()){
        return childs.at(n);
    }
    return NULL;
}

void ZipFileItem::addChild(ZipFileItem *child){
    childs.push_back(child);

}

void ZipFileItem::setZipFileParams(ZipFileAttr *file_attr){
    uncompress_size+=file_attr->uncompress_size;
    compress_size+=file_attr->compress_size;
    fillZipDate(date,file_attr);
    if(parent!=NULL){
        parent->incrementSize(file_attr->uncompress_size,file_attr->compress_size);//увеличить размер родителя с учетом размера дочернего элемента
        parent->incrementTotalChildCount();//увеличить общее количество элементов родителя
    }
}
void ZipFileItem::setType(uint32_t type){
   this->type=type;
}

void ZipFileItem::incrementTotalChildCount(){
    total_child++;
    if(parent!=NULL){
        parent->incrementTotalChildCount();
    }
}

void ZipFileItem::incrementSize(uint32_t uncompr_size, uint32_t compr_size){
    uncompress_size+=uncompr_size;
    compress_size+=compr_size;
    if(parent!=NULL){
        parent->incrementSize(uncompr_size,compr_size);//учитывается только добавка к размеру
    }
}

void ZipFileItem::addChild(string  name, uint32_t type ,ZipFileAttr * file_attr){
    ZipFileItem * parent = getRoot();
    ZipFileItem * item = NULL;
    vector<string> item_names=str_split(name,'/');
    vector<string>::iterator iter=item_names.begin();
    while(iter!=item_names.end()){
        string item_name=*iter;
        if((item=parent->isItemExist(item_name))==NULL){
            item=new ZipFileItem(item_name,parent);
            parent->addChild(item);
        }
        parent=item;
        iter++;
    }
    item->setType(type);
    item->setZipFileParams(file_attr);
}

void ZipFileItem::clear()
{
    vector<ZipFileItem*>::iterator iter=childs.begin();
    while(iter!=childs.end()){//удаление дочерних элементов
        delete *iter;
        iter++;
    }
    childs.clear();
}

ZipFileItem * ZipFileItem::getRoot()
{
    ZipFileItem * item=this;
    while(item->getParent()!=0){
        item=item->getParent();
    }
    return item;
}
ZipFileItem * ZipFileItem::isItemExist(string name)
{
    for(unsigned int i=0;i<getChildCount();i++)
    {
        ZipFileItem * item=getChild(i);
        if(!item->getName().compare(name)){
            return item;
        }
    }
    return NULL;

}






