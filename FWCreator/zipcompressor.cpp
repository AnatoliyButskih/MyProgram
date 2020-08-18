/* 
 * File:   ZipCompressor.cpp
 * Author: butskih
 * 
 * Created on 8 Февраль 2011 г., 16:32
 */

#include "zipcompressor.h"
#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#define THIS_FOLDER _S_IFDIR
#define THIS_FILE   _S_IFREG  
#else
#include <dirent.h>
#define THIS_FOLDER __S_IFDIR
#define THIS_FILE   __S_IFREG  
#endif
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>//для Windows параметр другой
#include <time.h>
#include <sstream>
#include <iomanip>




#define CHUNK           16384   //размеры буфера чтения/записи zip
#define WINDOW_BITS     -15     //компрессия без добавления gzip заголовка
#define MEM_LEVEL       8       //использование памяти по умолчанию
#define IS_FILE         10      //входной параметр является файлом
#define IS_FOLDER       11      //входной параметр является папкой
#define CRC32_BUFF      4096//размер буфера вычисления контрольной суммы



#define REV_BE(w) (((w)>>24)+(((w)>>8)&0xff00)+(((w)&0xff00)<<8)+(((w)&0xff)<<24))//переворачиваем слово

using namespace std;
const static uint32_t crc32_table[256] =/**/
{
    0x00000000L, 0x96300777L, 0x2c610eeeL, 0xba510999L, 0x19c46d07L,
    0x8ff46a70L, 0x35a563e9L, 0xa395649eL, 0x3288db0eL, 0xa4b8dc79L,
    0x1ee9d5e0L, 0x88d9d297L, 0x2b4cb609L, 0xbd7cb17eL, 0x072db8e7L,
    0x911dbf90L, 0x6410b71dL, 0xf220b06aL, 0x4871b9f3L, 0xde41be84L,
    0x7dd4da1aL, 0xebe4dd6dL, 0x51b5d4f4L, 0xc785d383L, 0x56986c13L,
    0xc0a86b64L, 0x7af962fdL, 0xecc9658aL, 0x4f5c0114L, 0xd96c0663L,
    0x633d0ffaL, 0xf50d088dL, 0xc8206e3bL, 0x5e10694cL, 0xe44160d5L,
    0x727167a2L, 0xd1e4033cL, 0x47d4044bL, 0xfd850dd2L, 0x6bb50aa5L,
    0xfaa8b535L, 0x6c98b242L, 0xd6c9bbdbL, 0x40f9bcacL, 0xe36cd832L,
    0x755cdf45L, 0xcf0dd6dcL, 0x593dd1abL, 0xac30d926L, 0x3a00de51L,
    0x8051d7c8L, 0x1661d0bfL, 0xb5f4b421L, 0x23c4b356L, 0x9995bacfL,
    0x0fa5bdb8L, 0x9eb80228L, 0x0888055fL, 0xb2d90cc6L, 0x24e90bb1L,
    0x877c6f2fL, 0x114c6858L, 0xab1d61c1L, 0x3d2d66b6L, 0x9041dc76L,
    0x0671db01L, 0xbc20d298L, 0x2a10d5efL, 0x8985b171L, 0x1fb5b606L,
    0xa5e4bf9fL, 0x33d4b8e8L, 0xa2c90778L, 0x34f9000fL, 0x8ea80996L,
    0x18980ee1L, 0xbb0d6a7fL, 0x2d3d6d08L, 0x976c6491L, 0x015c63e6L,
    0xf4516b6bL, 0x62616c1cL, 0xd8306585L, 0x4e0062f2L, 0xed95066cL,
    0x7ba5011bL, 0xc1f40882L, 0x57c40ff5L, 0xc6d9b065L, 0x50e9b712L,
    0xeab8be8bL, 0x7c88b9fcL, 0xdf1ddd62L, 0x492dda15L, 0xf37cd38cL,
    0x654cd4fbL, 0x5861b24dL, 0xce51b53aL, 0x7400bca3L, 0xe230bbd4L,
    0x41a5df4aL, 0xd795d83dL, 0x6dc4d1a4L, 0xfbf4d6d3L, 0x6ae96943L,
    0xfcd96e34L, 0x468867adL, 0xd0b860daL, 0x732d0444L, 0xe51d0333L,
    0x5f4c0aaaL, 0xc97c0dddL, 0x3c710550L, 0xaa410227L, 0x10100bbeL,
    0x86200cc9L, 0x25b56857L, 0xb3856f20L, 0x09d466b9L, 0x9fe461ceL,
    0x0ef9de5eL, 0x98c9d929L, 0x2298d0b0L, 0xb4a8d7c7L, 0x173db359L,
    0x810db42eL, 0x3b5cbdb7L, 0xad6cbac0L, 0x2083b8edL, 0xb6b3bf9aL,
    0x0ce2b603L, 0x9ad2b174L, 0x3947d5eaL, 0xaf77d29dL, 0x1526db04L,
    0x8316dc73L, 0x120b63e3L, 0x843b6494L, 0x3e6a6d0dL, 0xa85a6a7aL,
    0x0bcf0ee4L, 0x9dff0993L, 0x27ae000aL, 0xb19e077dL, 0x44930ff0L,
    0xd2a30887L, 0x68f2011eL, 0xfec20669L, 0x5d5762f7L, 0xcb676580L,
    0x71366c19L, 0xe7066b6eL, 0x761bd4feL, 0xe02bd389L, 0x5a7ada10L,
    0xcc4add67L, 0x6fdfb9f9L, 0xf9efbe8eL, 0x43beb717L, 0xd58eb060L,
    0xe8a3d6d6L, 0x7e93d1a1L, 0xc4c2d838L, 0x52f2df4fL, 0xf167bbd1L,
    0x6757bca6L, 0xdd06b53fL, 0x4b36b248L, 0xda2b0dd8L, 0x4c1b0aafL,
    0xf64a0336L, 0x607a0441L, 0xc3ef60dfL, 0x55df67a8L, 0xef8e6e31L,
    0x79be6946L, 0x8cb361cbL, 0x1a8366bcL, 0xa0d26f25L, 0x36e26852L,
    0x95770cccL, 0x03470bbbL, 0xb9160222L, 0x2f260555L, 0xbe3bbac5L,
    0x280bbdb2L, 0x925ab42bL, 0x046ab35cL, 0xa7ffd7c2L, 0x31cfd0b5L,
    0x8b9ed92cL, 0x1daede5bL, 0xb0c2649bL, 0x26f263ecL, 0x9ca36a75L,
    0x0a936d02L, 0xa906099cL, 0x3f360eebL, 0x85670772L, 0x13570005L,
    0x824abf95L, 0x147ab8e2L, 0xae2bb17bL, 0x381bb60cL, 0x9b8ed292L,
    0x0dbed5e5L, 0xb7efdc7cL, 0x21dfdb0bL, 0xd4d2d386L, 0x42e2d4f1L,
    0xf8b3dd68L, 0x6e83da1fL, 0xcd16be81L, 0x5b26b9f6L, 0xe177b06fL,
    0x7747b718L, 0xe65a0888L, 0x706a0fffL, 0xca3b0666L, 0x5c0b0111L,
    0xff9e658fL, 0x69ae62f8L, 0xd3ff6b61L, 0x45cf6c16L, 0x78e20aa0L,
    0xeed20dd7L, 0x5483044eL, 0xc2b30339L, 0x612667a7L, 0xf71660d0L,
    0x4d476949L, 0xdb776e3eL, 0x4a6ad1aeL, 0xdc5ad6d9L, 0x660bdf40L,
    0xf03bd837L, 0x53aebca9L, 0xc59ebbdeL, 0x7fcfb247L, 0xe9ffb530L,
    0x1cf2bdbdL, 0x8ac2bacaL, 0x3093b353L, 0xa6a3b424L, 0x0536d0baL,
    0x9306d7cdL, 0x2957de54L, 0xbf67d923L, 0x2e7a66b3L, 0xb84a61c4L,
    0x021b685dL, 0x942b6f2aL, 0x37be0bb4L, 0xa18e0cc3L, 0x1bdf055aL,
    0x8def022dL
};



#pragma pack (push,1)
struct  ZipCentralDir{//параметры центральной директории
    uint32_t signature;//сигнатура заголовка файла
    uint16_t made_by;//версия в которой созданно
    uint16_t extract_vers;//версия необходимая для извлечения
    uint16_t bit_flag;
    uint16_t compr_meth;//метод сжатия
    uint16_t time_mod;//время модификации
    uint16_t date_mod;//дата модификации
    uint32_t crc32_sum;//контрольная сумма
    uint32_t compress_size;//размер сжатого файла
    uint32_t uncompress_size;//размер разархивированного файла
    uint16_t name_length;//длинна имени файла
    uint16_t field_length;//длинна дополнительного поля (по умолчанию 0)
    uint16_t comment_length;//длинна дополнительного поля (по умолчанию 0)
    uint16_t disk_number;//по умолчанию 0
    uint16_t int_attr;
    uint32_t ext_attr;
    uint32_t relative_offset;


};//__attribute__((packed));
#pragma pack(pop)

#pragma pack (push,1)
struct ZipEndCentralDir{//окончание центальной директории
    uint32_t signature;//сигнатура заголовка файла
    uint16_t disk_numb;
    uint16_t disk_starts;
    uint16_t number_cdir_on_disk;
    uint16_t total_cdir;
    uint32_t size_cdir;
    uint32_t offset_cdir;
    uint16_t comment_length;
};//__attribute__((packed));
#pragma pack(pop)


//static uint32_t offset_central_dir=0;//смещение до central directory file header
//static uint32_t size_central_dir=0;//размер central directory file header
//static uint32_t total_records_central_dir=0;//количество записей в central directory file header
//static uint32_t zip_file_offset=0;//смещение ZIP секции в общем файле
static z_stream strm;//структура zlib
//string firmware_path;//путь до директории содержащей firmware




void str_replase_all(string in_str, string orig, string repl){
for(size_t index=0; index=in_str.find(orig, index), index!=string::npos;)
{
    in_str.replace(index, orig.length(), repl);
    index+=repl.length();
  }

}



uint32_t crc32_reverse(unsigned char* buf, size_t len, uint32_t crc32)//вычисление crc32 суммы массива
{
    while(len--)
    {
        crc32 =  (crc32 << 8) ^ crc32_table[(crc32>>24 ^ *buf++) & 0xFF] ;
    }
    return crc32;
}

uint32_t zip_get_file_type(string file_name){
    FILE * file;
    file=fopen(file_name.c_str(),"rb");
    if(file==NULL){
        return ZipFileItem::IS_UNKN;
    }
    char * file_header=new char[4];
    fread (file_header,1,4,file);
    fclose(file);
    uint32_t  bin_header=*((uint32_t *)file_header);
    uint16_t  script_header=*((uint16_t *)file_header);
    delete [] file_header;



    if(bin_header==0x464C457F){//идентификатор запускаемого файла "ELF"
        return ZipFileItem::IS_BIN;
    }
    if(script_header==0x2123){//идентификатор скрипта "#!"
        return ZipFileItem::IS_SCRIPT;
    }
    size_t ext_pos=file_name.find_last_of(".");
    string ext;
    if(ext_pos!=string::npos){
        ext=file_name.substr(ext_pos+1);
    }

    if(!ext.compare("cfg"))
    {
        return ZipFileItem::IS_CFG;
    }
    if(!ext.compare("log")){
        return ZipFileItem::IS_LOG;
    }
    return ZipFileItem::IS_UNKN;
}

void zip_get_file_attr(string in_file_name, ZipFileAttr* file_attr)//получение атрибутов файла
{
    //FILE * input=fopen(file_name.c_str(),"r");
    struct stat file_stat;//параметры файла
    struct tm * time_opt;//временнфе параметры
	int ret;
    ret=stat(in_file_name.c_str(), &file_stat);
	if(ret==-1){
		return;
	}

    file_attr->uncompress_size=(uint32_t)file_stat.st_size;

    time_opt = localtime(&file_stat.st_mtime);
	
	
    uint16_t year=(uint16_t)(time_opt->tm_year);
    if (year>=1980)
        year-=1980;
    else if (year>=80)
        year-=80;
    uint16_t month=time_opt->tm_mon+1;
    uint16_t day=time_opt->tm_mday;

    uint16_t hour=time_opt->tm_hour;
    uint16_t minute=time_opt->tm_min;
    uint16_t second=time_opt->tm_sec/2;

    file_attr->date_mod=((year & 0x3F)<<9) | ((month & 0xF)<<5) | (day & 0x1F);
    file_attr->time_mod=((hour & 0x1F)<<11) | ((minute & 0x3F)<<5) | (second & 0x1F);

}

int get_file_type(string in_file_name){
    struct stat file_stat;//параметры файла
	
    stat(in_file_name.c_str(), &file_stat);
    if(file_stat.st_mode & THIS_FOLDER){
        return IS_FOLDER;
    }
    if(file_stat.st_mode & THIS_FILE){
        return IS_FILE;
    }

}

void ZipCompressor::zip_write_central_header()
{
    fseek (out_file , 0 , SEEK_END);
    offset_central_dir=ftell(out_file)-zip_file_offset;
    size_central_dir=0;
    total_records_central_dir=0;

    fseek (out_file , zip_file_offset, SEEK_SET);
    ZipFileAttr * attr = new ZipFileAttr;
    ZipCentralDir * cdir=new ZipCentralDir;
    char * name_buff=new char[4096];//буфер для строки имени
    uint32_t seek_pos=0;
    while(fread(attr,1,sizeof(ZipFileAttr),out_file)!=0 && attr->signature==FILE_HEAD_SIGN){
        cdir->signature=CENTRAL_DIR_SIGN;
        cdir->made_by=0x14;
        cdir->extract_vers=0x0A;
        cdir->bit_flag=0x0;
        cdir->compr_meth=0x08;
        cdir->time_mod=attr->time_mod;
        cdir->date_mod=attr->date_mod;
        cdir->crc32_sum=attr->crc32_sum;
        cdir->compress_size=attr->compress_size;
        cdir->uncompress_size=attr->uncompress_size;
        cdir->name_length=attr->name_length;
        cdir->field_length=0x0;
        cdir->comment_length=0x0;
        cdir->disk_number=0x0;
        cdir->int_attr=0x0;
        cdir->relative_offset=ftell(out_file)-sizeof(ZipFileAttr)-zip_file_offset;//смещение до заголовка файла + учитываются данные перед ZIP секцией
        fread(name_buff,1,cdir->name_length,out_file);//считали имя файла
        name_buff[cdir->name_length-1]=='/' ? cdir->ext_attr=0x10 : cdir->ext_attr=0x0;// сообщаем что директория есть директория


        fseek(out_file,0,SEEK_END);

        size_central_dir+=fwrite(cdir,1,sizeof(ZipCentralDir),out_file);
        size_central_dir+=fwrite(name_buff,1,cdir->name_length,out_file);
        total_records_central_dir++;

        seek_pos+= sizeof(ZipFileAttr)+attr->name_length+attr->compress_size+attr->field_length;
        fseek (out_file , zip_file_offset+seek_pos , SEEK_SET );
    }

}

void ZipCompressor::zip_write_end_central_dir()
{
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime (&rawtime);
    string zip_comment="Created by Infotrans ";
    

    unsigned int year=timeinfo->tm_year;

    if (year<1900){
        year+=1900;
    }
    unsigned int month=timeinfo->tm_mon+1;
    unsigned int day=timeinfo->tm_mday;

    unsigned int hour=timeinfo->tm_hour;
    unsigned int minute=timeinfo->tm_min;
    unsigned int second=timeinfo->tm_sec;

    stringstream str_stream;
    str_stream.clear();
    str_stream<<"Created by Infotrans ";
    str_stream<<setw(2)<<setfill('0');
    str_stream<<hour<<setw(1)<<":" \
    <<setw(2)<<setfill('0')<<minute<<setw(1)<<":" \
    <<setw(2)<<setfill('0')<<second<<setw(1)<<" " \
    <<setw(2)<<setfill('0')<<day<<setw(1)<<"." \
    <<setw(2)<<setfill('0')<<month<<setw(1)<<"." \
    <<setw(4)<<setfill('0')<<year<<setw(1);

    getline(str_stream,zip_comment);

    fseek (out_file , 0 , SEEK_END);
    ZipEndCentralDir * end_cdir=new ZipEndCentralDir;
    end_cdir->signature=END_DIR_SIGN;
    end_cdir->disk_numb=0x0;
    end_cdir->disk_starts=0x0;
    end_cdir->number_cdir_on_disk=total_records_central_dir;
    end_cdir->total_cdir=total_records_central_dir;
    end_cdir->size_cdir=size_central_dir;
    end_cdir->offset_cdir=offset_central_dir;
    end_cdir->comment_length=zip_comment.length();
    fwrite(end_cdir,1,sizeof(ZipEndCentralDir),out_file);
    fwrite(zip_comment.c_str(),1,end_cdir->comment_length,out_file);//запись комментариев к файлу
}

int ZipCompressor::zip_compress_file(string in_file_name)//жмем файл
{
    int ret, flush;//возвращаемые значения
    size_t have;
    string zip_file_name;
    if(in_file_name.find_first_of(firmware_path)==0){//удаляем  путь не относящийся к архиву
        zip_file_name=in_file_name.substr(firmware_path.length()+1);
    }else{
        return Z_ERRNO;
    }


    if(out_file==NULL)
    {
        return Z_ERRNO;
    }
    FILE * in_file;

    in_file=fopen(in_file_name.c_str(),"rb");

    if(in_file==NULL)
    {
        return Z_ERRNO;
    }

    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    ret=deflateInit2(&strm,Z_DEFAULT_COMPRESSION,Z_DEFLATED, WINDOW_BITS, MEM_LEVEL, Z_DEFAULT_STRATEGY);
    if (ret != Z_OK)
    {
        return ret;
    }

    

    unsigned char in[CHUNK];
    unsigned char out[CHUNK];

    
    uint32_t crc32=(uint32_t)0xFFFFFFFF;//начальное значение crc32
    uint32_t compress_size=0;//размер сжатого архива
    uint32_t file_type=zip_get_file_type(in_file_name);//тип файла
    ZipFileAttr * file_attr=new ZipFileAttr;
    file_attr->signature=FILE_HEAD_SIGN;//сигнатура
    file_attr->extract_vers=0x0A;//минимально необходимая версия для разархифирования
    file_attr->bit_flag=0x0;
    file_attr->compr_meth=0x08;//метод компресии deflate
    zip_get_file_attr(in_file_name,file_attr);
    file_attr->name_length=(uint16_t)zip_file_name.length();
    file_attr->field_length=sizeof(file_type);

    fseek(out_file,0,SEEK_END);//переходим к концу файла для записи
    uint32_t file_pos=ftell(out_file);//позиция в исходном zip файле

    fwrite(file_attr,1,sizeof(ZipFileAttr),out_file);//запись заголовка файла
    fwrite(zip_file_name.c_str(),1,file_attr->name_length,out_file);//записывем имя файла
    fwrite(&file_type,1,sizeof(file_type),out_file);//записывем  тип файла



//---------------
     /* compress until end of file */
    do {
        strm.avail_in = fread(in, 1, CHUNK, in_file);
        if (ferror(in_file)) {
            (void)deflateEnd(&strm);
            return Z_ERRNO;
        }
        crc32=crc32_reverse(in,strm.avail_in,crc32);//вычисяем crc32 для данных

        flush = feof(in_file) ? Z_FINISH : Z_NO_FLUSH;
        strm.next_in = in;

        /* run deflate() on input until output buffer not full, finish
           compression if all of source has been read in */
        do {
            strm.avail_out = CHUNK;
            strm.next_out = out;
            ret = deflate(&strm, flush);    /* no bad return value */
            assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
            //strm->
            have = CHUNK - strm.avail_out;
            if (fwrite(out, 1, have, out_file) != have || ferror(out_file)) {
                (void)deflateEnd(&strm);
                return Z_ERRNO;
            }
            compress_size+=have;
        } while (strm.avail_out == 0);
       assert(strm.avail_in == 0);     /* all input will be used */

        /* done when last data in file processed */
    } while (flush != Z_FINISH);
    assert(ret == Z_STREAM_END);        /* stream will be complete */



    file_attr->crc32_sum=REV_BE(~crc32);
    file_attr->compress_size=compress_size;

    fseek(out_file,file_pos,SEEK_SET);
    fwrite(file_attr,1,sizeof(ZipFileAttr),out_file);
    fclose(in_file);

    (void)deflateEnd(&strm);

    return Z_OK;
}

int ZipCompressor::zip_compress_folder(string in_folder_name)//жмем папку
{
    
    string zip_folder_name;

    if(in_folder_name.find_first_of(firmware_path)==0){//удаляем  путь не относящийся к архиву
        zip_folder_name=in_folder_name.substr(firmware_path.length()+1);
    }else{
        return Z_ERRNO;
    }
   if(zip_folder_name.find_last_of("/")!=zip_folder_name.length()-1){
        zip_folder_name.append("/");
    }
	
    uint32_t file_type=ZipFileItem::IS_DIR;
    ZipFileAttr * file_attr=new ZipFileAttr;
    file_attr->signature=FILE_HEAD_SIGN;//сигнатура
    file_attr->extract_vers=0x0A;//минимально необходимая версия для разархифирования
    file_attr->bit_flag=0x0;
    file_attr->compr_meth=0x00;//метод компресии deflate
    zip_get_file_attr(in_folder_name,file_attr);
    file_attr->crc32_sum=0;
    file_attr->compress_size=0x0;
    file_attr->uncompress_size=0x0;

    file_attr->name_length=(uint16_t)zip_folder_name.length();
    file_attr->field_length=sizeof(file_type);

    fseek(out_file,0,SEEK_END);
    fwrite(file_attr,1,sizeof(ZipFileAttr),out_file);//запись заголовка файла
    fwrite(zip_folder_name.c_str(),1,file_attr->name_length,out_file);//записываем имя папки
    fwrite(&file_type,1,sizeof(file_type),out_file);//записываем имя папки

    return Z_OK;

    //fflush(out_file);
}

#ifdef _WIN32
int ZipCompressor::recurs_dir(string dir_name)
{
	 LPWIN32_FIND_DATAA ffd=new WIN32_FIND_DATAA;
	 string find_str=dir_name+"/*";
	 HANDLE hSearch=FindFirstFileA(find_str.c_str(), ffd);//нашли первый файл
	 do{
		string name=dir_name;
		if((ffd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && strcmp(ffd->cFileName,".") && strcmp(ffd->cFileName,"..")){
			name=name.append("/").append(ffd->cFileName);
                        if(zip_compress_folder(name)!=Z_OK){
                            return -1;
                        }
                        if(recurs_dir(name)){
                            return -1;
                        }
		}else if((ffd->dwFileAttributes != FILE_ATTRIBUTE_REPARSE_POINT) && strcmp(ffd->cFileName,".") && strcmp(ffd->cFileName,"..")){//если не ссылка
			name=name.append("/").append(ffd->cFileName);
                        if(zip_compress_file(name)!=Z_OK){
                           return -1;
                        }

		}
	 }while(FindNextFileA (hSearch, ffd));
     return 0;
}
#else
int ZipCompressor::recurs_dir(string dir_name)
{
    
    DIR * dir;
    dirent * dir_data;
    dir=opendir(dir_name.c_str());
    if(dir==NULL){
        return -1;
    }
    
    while((dir_data=readdir(dir))!=NULL)
    {
         string name=dir_name;
         switch(dir_data->d_type){
         case DT_DIR:
         {
              name=name.append("/").append(dir_data->d_name);
              
              if(strcmp(dir_data->d_name,".")!=0 && strcmp(dir_data->d_name,"..")!=0){
                  
                    if(zip_compress_folder(name)!=Z_OK){
                        return -1;
                    }
                    if(recurs_dir(name)){
                        return -1;
                    }

              }
         }
         break;
         case DT_REG:
                name=name.append("/").append(dir_data->d_name);
                if(zip_compress_file(name)!=Z_OK){
                    return -1;
                }
         break;

        }
     }
    return 0;
}
#endif

ZipCompressor::ZipCompressor(){

}

int ZipCompressor::zip_compess(string in_folder_name,FILE * out_file)
{

    firmware_path=in_folder_name;
    this->out_file=out_file;




    if(out_file==NULL)
    {
        return Z_ERRNO;
    }
    fseek(out_file,0,SEEK_END);
    zip_file_offset=ftell(out_file);
    
    
    int in_type=get_file_type(in_folder_name);
    switch(in_type){
        case IS_FILE:
            if(zip_compress_file(in_folder_name)!=Z_OK){
                return -1;
            }
            break;
        case IS_FOLDER:
            //zip_compress_folder(in, out_file);//сжимаем начальную папку
            if(recurs_dir(in_folder_name)){
                return -1;
            }
            break;

    }

    //fclose(out_file);
    //out_file=fopen(out_file_name.c_str(),"r+");

    zip_write_central_header();
    zip_write_end_central_dir();

    
    return 0;
}

uint32_t ZipCompressor::crc32_calc(unsigned char *buf, size_t len, uint32_t crc32)
{
    return crc32_reverse(buf, len, crc32);
}
uint32_t ZipCompressor::crc32_calc(FILE *file, uint32_t offset, uint32_t length, uint32_t crc32){
    fseek(file,offset,SEEK_SET);//переход к данным секции
    unsigned char * crc32_buff= new unsigned char[CRC32_BUFF];
    while(length)
    {
        size_t read_size=0;
        length >= CRC32_BUFF ? read_size=CRC32_BUFF : read_size=length;

        fread(crc32_buff,1,read_size,file);
        crc32=crc32_reverse(crc32_buff,read_size,crc32);
        length-=read_size;
    }
    delete [] crc32_buff;
    return REV_BE(~crc32);
}

ZipFileItem* ZipCompressor::zip_files(FILE *file, uint32_t offset, uint32_t length)
{
    in_file=file;
    zip_root_item=new ZipFileItem("root");
    fseek(in_file,offset,SEEK_SET);
    ZipFileAttr file_attr;

    while(fread(&file_attr,1,sizeof(ZipFileAttr),in_file)!=0 && file_attr.signature==FILE_HEAD_SIGN){
        char * file_name= new char[file_attr.name_length];
        uint32_t file_type;
        fread(file_name,1,file_attr.name_length,in_file);//читаем имя файла
        fread(&file_type,1,sizeof(file_type),in_file);//читаем тип файла ext field
        string name_str(file_name,file_attr.name_length);
        delete [] file_name;
        zip_root_item->addChild(name_str,file_type,&file_attr);//добавление элемента

        size_t seek_pos=file_attr.compress_size;//размер смещения (с учетом дополнительного поля)
        fseek(in_file,seek_pos,SEEK_CUR);
    }
    return zip_root_item;

}

int ZipCompressor::zip_decompess(string out_folder_name, FILE *zip_file, uint32_t offset, uint32_t length)
{
    decompress_path=out_folder_name;
    in_file=zip_file;
    fseek(in_file,offset,SEEK_SET);
    ZipFileAttr file_attr;

    while(fread(&file_attr,1,sizeof(ZipFileAttr),in_file)!=0 && file_attr.signature==FILE_HEAD_SIGN){
        char * file_name= new char[file_attr.name_length];
        fread(file_name,1,file_attr.name_length,in_file);//читаем имя файла
        string name_str(file_name,file_attr.name_length);//создаем string с именем файла
        delete [] file_name;
        uint32_t file_offset=ftell(in_file)+file_attr.field_length;

        if(name_str.find_last_of('/')==name_str.length()-1 && file_attr.uncompress_size==0){
            if(zip_decompress_folder(name_str)!=Z_OK){
                return -1;
            }
        }else{
            if(zip_decompress_file(name_str,file_offset,&file_attr)!=Z_OK){
                return -1;
            }
        }

        fseek(in_file,file_offset+file_attr.compress_size,SEEK_SET);
    }

        return 0;

}

void ZipCompressor::create_file_path(string file_name,bool is_dir){
    vector<string> name= ZipFileItem::str_split(file_name,'/');
    int max_i=is_dir ? name.size() : name.size()-1;
    string file_path=decompress_path;
    for(int i=0;i<max_i;i++){
        file_path=file_path.append("/").append(name.at(i));
#ifdef _WIN32
                   _mkdir(file_path.c_str());//создание директории в Windows
#else
		   mkdir(file_path.c_str(),0755);//создание директории в UINIX
#endif

    }
}

int ZipCompressor::zip_decompress_file(string file_name,uint32_t offset, ZipFileAttr * file_attr)
{
    create_file_path(file_name,false);
    if(decompress_path.find_last_of("/")!=decompress_path.length()-1){
        decompress_path.append("/");
    }
    string full_file_path=decompress_path+file_name;
    FILE * decompr_file = fopen(full_file_path.c_str(),"w+b");
    if(decompr_file==NULL){
        return Z_ERRNO;
    }
    fseek(in_file,offset,SEEK_SET);
    int ret;
    unsigned have;


    unsigned char in[CHUNK];
    unsigned char out[CHUNK];

    /* allocate inflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    ret = inflateInit2(&strm, WINDOW_BITS);
    if (ret != Z_OK)
        return ret;
    uint32_t already_read=0;
    uint32_t crc32=0xFFFFFFFF;

    /* decompress until deflate stream ends or end of file */
    do {
        uint32_t read_size=already_read+CHUNK>file_attr->compress_size ? file_attr->compress_size-already_read : CHUNK;
        strm.avail_in = fread(in, 1, read_size, in_file);
        already_read+=strm.avail_in;
        if (ferror(in_file)) {
            (void)inflateEnd(&strm);
            fclose(decompr_file);
            return Z_ERRNO;
        }
        if (strm.avail_in == 0)
            break;
        strm.next_in = in;

        /* run inflate() on input until output buffer not full */
        do {
            strm.avail_out = CHUNK;
            strm.next_out = out;
            ret = inflate(&strm, Z_NO_FLUSH);
            assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
            switch (ret) {
            case Z_NEED_DICT:
                ret = Z_DATA_ERROR;     /* and fall through */
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                (void)inflateEnd(&strm);
                fclose(decompr_file);
                return ret;
            }
            have = CHUNK - strm.avail_out;
            crc32=crc32_reverse(out,have,crc32);//вычисяем crc32 для данных
            if (fwrite(out, 1, have, decompr_file) != have || ferror(decompr_file)) {
                (void)inflateEnd(&strm);
                fclose(decompr_file);
                return Z_ERRNO;
            }
        } while (strm.avail_out == 0);

        /* done when inflate() says it's done */
    } while (ret != Z_STREAM_END && already_read<file_attr->compress_size);

    /* clean up and return */
    (void)inflateEnd(&strm);



    fclose(decompr_file);
    if(file_attr->crc32_sum!=REV_BE(~crc32)){
        return Z_DATA_ERROR;
    }
    return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}

int ZipCompressor::zip_decompress_folder(string folder_name){
    create_file_path(folder_name,true);
    return Z_OK;
}






