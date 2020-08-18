#ifndef FWREADER_H
#define FWREADER_H
#include <stdio.h>
#include <stdint.h>
#include <map>
#include <QString>
#include <QObject>
#include <QList>
#include <QStringList>
#include <QObject>
#include "fwdata.h"
#include "tempfile.h"
#include "zipcompressor.h"

class FWReader: public QObject
{
    Q_OBJECT
public:
    FWReader();
    ~FWReader();
    int init_read(QString file_name);
    bool isInit();//статус готовности ридера
    uint32_t getSectionOffset(uint16_t section_type);
    const SectionHeader * getSectionHeader(uint16_t section_type);
    ZipFileItem * getZipFileStructure();
    QString getVersionFW();
    QString getPathOnServer();
    QString getHostName();
    QStringList getIP_SSH();
    uint16_t getPortSSH();
    QString getLoginSSH();
    QString getPasswdSSH();
    QString getComment();
    QStringList getConfirmList();
    int extractFW(QString extract_path);
    QString getMagic();
    FILE * getFileFW();
    void clear();
    QString a;

/* Ошибки чтения*/

    const static int NO_READ_ERROR        = 0x10;
    const static int SIGNATURE_READ_ERROR = 0x11;
    const static int CRC32_READ_ERROR     = 0x12;
    const static int OPEN_ERROR           = 0x13;
    const static int SECTION_ERROR        = 0x14;

/*Ошибки получения параметров*/
    const static int ERROR_GET_VERSION_FW       = 0x110;
    const static int ERROR_GET_PATH_ON_SERVER   = 0x111;
    const static int ERROR_GET_HOSTNAME         = 0x112;
    const static int ERROR_GET_SSH_IP           = 0x113;
    const static int ERROR_GET_SSH_PORT         = 0x114;
    const static int ERROR_GET_SSH_LOGIN        = 0x115;
    const static int ERROR_GET_SSH_PASSWORD     = 0x116;
    const static int ERROR_GET_COMMENT          = 0x117;
    const static int ERROR_GET_CONFIRM_LIST     = 0x118;
/*Ошибка извлечения из прошивки данных*/
    const static int ERROR_EXTRACT_DATA         = 0x119;
/*Статус извлечения*/
    const static int SUCCESS_EXTRACT_DATA       = 0x1110;

signals:
    void readError(int code);//ошибка чтения файла
    void readStatus(int status);




private:

    TempFile tmp;//отвечает за создание временного файла
    FILE * plainFW(FILE * chiper_file);
    bool init_status;
    FILE * in_file;
    uint32_t chiper_key[8];
    map<uint32_t,SectionHeader> sections;//содержит пару <смещение до секции>:<заголовок секции>
    ZipFileItem * zip_files;
    ZipCompressor * zip_compressor;


};

#endif // FWREADER_H
