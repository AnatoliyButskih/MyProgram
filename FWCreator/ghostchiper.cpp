#include "ghostchiper.h"
#include <string.h>
#include <iostream>
#include <stdio.h>

const uint8_t GhostChiper::REPLACE_TABLE[REPLASE_TABLE_SIZE]=//таблица замен по умолчанию
{
        4,  10, 9,  2,  13, 8,  0,  14, 6,  11, 1,  12, 7,  15, 5,  3,
        14, 11, 4,  12, 6,  13, 15, 10, 2,  3,  8,  1,  0,  7,  5,  9,
        5,  8,  1,  13, 10, 3,  4,  2,  14, 15, 12, 7,  6,  0,  9,  11,
        7,  13, 10, 1,  0,  8,  9,  15, 14, 4,  6,  12, 11, 2,  5,  3,
        6,  12, 7,  1,  5,  15, 13, 8,  4,  10, 9,  14, 0,  3,  11, 2,
        4,  11, 10, 0,  7,  2,  1,  13, 3,  6,  8,  5,  9,  12, 15, 14,
        13, 11, 4,  1,  3,  15, 5,  9,  0,  10, 14, 7,  6,  8,  2,  12,
        1,  15, 13, 0,  5,  7,  10, 4,  9,  2,  3,  14, 6,  11, 8,  12
        };

uint8_t repl_table_ext[1024];//расширенная таблица замен



inline uint32_t  cyclic_shift_left32(uint32_t value, uint8_t shift){//циклический сдвиг 32-х битного
    return ((value << shift) | (value >> (32 - shift)));
}

void generate_ext_table(const uint8_t * repl_table){//генерация расширенной таблицы замен из обычной
    int l=0;
    for(int k=0;k<8;k+=2){
        for(int j=0;j<16;j++){
            for(int i=0;i<16;i++){
                repl_table_ext[l]=(repl_table[16*k+i] & 0x0F)|((repl_table[(k+1)*16+j]<<4) & 0xF0);
                l++;
            }
        }
    }
}

GhostChiper::GhostChiper(const uint32_t *key, const uint8_t * repl_table)
{
    generate_ext_table(repl_table);
    memcpy(this->key,key,sizeof(this->key));
}

uint64_t GhostChiper::ghost_gamma_codec(uint64_t gamma, char *buffer, size_t len,bool mode)
{
    while(len){
        gamma=ghost_coding(gamma);
        uint64_t plain=0;
        size_t gamma_size = len>sizeof(uint64_t) ? sizeof(uint64_t) : len;
        memcpy(&plain,buffer,gamma_size);//копируем данные в переменную
        uint64_t chiper=plain;//сохраняем шифрованные данные (используетя при расшифровке)
        plain^=gamma;
        memcpy(buffer,&plain,gamma_size);
        /*ВАЖНО в следующую итерацию передается ЗАШИФРОВАННЫЕ данные*/
        gamma=mode ? plain: chiper;

        buffer+=gamma_size;
        len-=gamma_size;
    }
    return gamma;
}


uint64_t GhostChiper::ghost_coding(uint64_t plain){

    for(int k=0;k<3;k++){
        for(int i=0;i<8;i++){
            plain=basic_chiper_step(plain,key[i]);

        }
    }
    for(int i=7;i>-1;i--){
        plain=basic_chiper_step(plain,key[i]);
    }



    return plain;
}



uint64_t GhostChiper::basic_chiper_step(uint64_t data, uint32_t key){
    uint64_t high=(uint32_t)(data>>32);
    uint32_t low=(uint32_t)(data);
    low+=key;//сложение по модулю 2^32
    uint8_t * blocs=(uint8_t*)&low;
    for(unsigned int i=0;i<sizeof(uint32_t);i++){//замена блоков
        blocs[i]=repl_table_ext[i*256+blocs[i]];
    }
    low=cyclic_shift_left32(low,11);
    low^=high;
    high=(uint32_t)(data);
    return (((uint64_t)high)<<32 | low);
}


