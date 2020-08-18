#ifndef ZIPFILEITEM_H
#define ZIPFILEITEM_H
#include <vector>
#include <string>
#include <stdint.h>
//#include <zipcompressor.h>

using namespace std;
struct ZipDate{
    uint32_t year;
    uint32_t month;
    uint32_t day;

    uint32_t second;
    uint32_t minute;
    uint32_t hour;
};

#pragma pack (push,1)
struct ZipFileAttr//параметры файла
{
   uint32_t signature;//сигнатура заголовка файла
   uint16_t extract_vers;//минимально необходимая для извлечения версия
   uint16_t bit_flag;//битовый флаг
   uint16_t compr_meth;//метод сжатия
   uint16_t time_mod;//время модификации
   uint16_t date_mod;//дата модификации
   uint32_t crc32_sum;//контрольная сумма
   uint32_t compress_size;//размер сжатого файла
   uint32_t uncompress_size;//размер разархивированного файла
   uint16_t name_length;//длинна имени файла
   uint16_t field_length;//длинна дополнительного поля (по умолчанию 0)

};
#pragma pack(pop)

class ZipFileItem
{
public:
    ZipFileItem(string name, ZipFileItem * parent=0);
    ~ZipFileItem();
    string getName() const;
    uint32_t getUncompressSize() const;
    uint32_t getCompressSize() const;
    uint32_t getTotalChildCount() const;
    uint32_t getType() const;
    ZipDate  getZipDate() const;
    size_t   getChildCount() const;
    ZipFileItem * getParent();
    ZipFileItem * getChild(size_t n);
    void addChild(ZipFileItem * child);
    void addChild(string  name, uint32_t type ,ZipFileAttr * file_attr);
    static vector<string> str_split(string & str, char delim);


    void clear();//очистка дочерних элементов
    ZipFileItem * getRoot();


    static const uint32_t IS_DIR     = 0x111;//файл является директорией
    static const uint32_t IS_BIN     = 0x112;//файл является бинарным
    static const uint32_t IS_SCRIPT  = 0x113;//файл является shell скриптом
    static const uint32_t IS_CFG     = 0x114;//файл является конфигурационным
    static const uint32_t IS_LOG     = 0x115;//файл является логом
    static const uint32_t IS_UNKN    = 0x116;//тип файла неизвестен

protected:
    void incrementSize(uint32_t uncompr_size,uint32_t compr_size);//увеличить размер родителя при добавлении дочернего элемента
    void setZipFileParams(ZipFileAttr * file_attr);//установить аттрибуты для файла
    void incrementTotalChildCount();//увеличить общее количество дочерних элементов
    void setType(uint32_t type);
private:
    ZipFileItem * isItemExist(string name);//проверяет существует ли дочерняя нода в данном с именем name
    string name;//имя файла
    uint32_t uncompress_size;//размер не сжатого файла
    uint32_t compress_size;//размер сжатого файла
    uint32_t type;//тип файла
    uint32_t total_child;//общее количество дочерних элементов
    ZipDate date;//дата модификации файла
    vector<ZipFileItem*> childs;
    ZipFileItem * parent;

};

#endif // ZIPFILEITEM_H
