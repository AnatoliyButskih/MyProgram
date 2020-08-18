#ifndef GHOSTCHIPER_H
#define GHOSTCHIPER_H
#include <stdint.h>
#include <string.h>
#define REPLASE_TABLE_SIZE  128//размер таблицы замены

class GhostChiper
{
public:
    GhostChiper( const uint32_t *key, const uint8_t * repl_table=REPLACE_TABLE);
    uint64_t ghost_gamma_codec(uint64_t gamma, char* buffer,size_t len,bool mode);
    uint64_t ghost_coding(uint64_t plain);//основной шаг кодирования информации
    const static uint8_t REPLACE_TABLE[REPLASE_TABLE_SIZE];
private:
    uint64_t basic_chiper_step(uint64_t data, uint32_t key);
    uint32_t key[8];
};

#endif // GHOSTCHIPER_H
