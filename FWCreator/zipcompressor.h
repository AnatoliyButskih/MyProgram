/* 
 * File:   ZipCompressor.h
 * Author: butskih
 *
 * Created on 8 Февраль 2011 г., 16:32
 */

#ifndef ZIPCOMPRESSOR_H
#define	ZIPCOMPRESSOR_H

#include <vector>
#include <string>
#include <stdint.h>
#include <zlib.h>
#include "zipfileitem.h"


using namespace std;




class ZipCompressor {
public:
    ZipCompressor();
    static uint32_t crc32_calc(FILE * file,uint32_t offset, uint32_t length,uint32_t crc32);//вычисление CRC32 куска файла
    static uint32_t crc32_calc(unsigned char* buf, size_t len, uint32_t crc32);//вычисление CRC32 произвольного буфера
    ZipFileItem* zip_files(FILE * file,uint32_t offset, uint32_t length);

    int zip_compess(string in_folder_name,FILE * out_file);
    int zip_decompess(string out_folder_name,FILE * zip_file, uint32_t offset, uint32_t length);
private:
    void zip_write_central_header();
    void zip_write_end_central_dir();
    int zip_compress_file(string in_file_name);
    int zip_compress_folder(string in_folder_name);
    int recurs_dir(string dir_name);
    void create_file_path(string file_name,bool is_dir);
    int zip_decompress_file(string file_name,uint32_t offset, ZipFileAttr * file_attr);
    int zip_decompress_folder(string folder_name);
    const static uint32_t FILE_HEAD_SIGN    = 0x04034b50;
    const static uint32_t CENTRAL_DIR_SIGN  = 0x02014b50;
    const static uint32_t END_DIR_SIGN      = 0x06054b50;


    FILE * out_file;
    FILE * in_file;
    uint32_t offset_central_dir;//смещение до central directory file header
    uint32_t size_central_dir;//размер central directory file header
    uint32_t total_records_central_dir;//количество записей в central directory file header
    uint32_t zip_file_offset;//смещение ZIP секции в общем файле прошивки
    string firmware_path;//путь до директории содержащей firmware
    string decompress_path;//путь до директории содержащей разархивирорванный патч
    ZipFileItem * zip_root_item;//структура содержащая файла zip архива


};

#endif	/* ZIPCOMPRESSOR_H */

