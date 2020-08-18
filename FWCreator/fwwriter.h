#ifndef FWWRITER_H
#define FWWRITER_H
#include <QStringList>
#include <QObject>
#include "fwdata.h"
#include "fwreader.h"
#include "zipcompressor.h"
#include "ghostchiper.h"
#include "tempfile.h"
class FWWriter : public QObject
{
 Q_OBJECT
public:
    FWWriter();
    int write_firmware(const FWData * fwdata, FWReader * fwreader=NULL);
    const static int ERROR_CREATE_TMP_FILE              = 0x10;//ошибка создания временного файла
    const static int ERROR_WRITE_FILE_HEADER            = 0x11;//ошибка записи заголовка файла
    const static int ERROR_WRITE_VERSION_SECTION        = 0x12;//ошибка записи секции версии файла
    const static int ERROR_WRITE_PATH_SECTION           = 0x13;//ошибка записи секции пути копирования на сервер
    const static int ERROR_WRITE_HOSTNAME_SECTION       = 0x14;//ошибка записи секции имени хоста
    const static int ERROR_WRITE_SSH_IP_SECTION         = 0x15;//ошибка записи секции IP адреса хоста назначения
    const static int ERROR_WRITE_SSH_PORT_SECTION       = 0x16;//ошибка записи секции порта SSH хоста назначения
    const static int ERROR_WRITE_SSH_LOGIN_SECTION      = 0x17;//ошибка записи секции имени пользователя SSH хоста назначения
    const static int ERROR_WRITE_SSH_PASSWD_SECTION     = 0x18;//ошибка записи секции пароля SSH хоста назначения
    const static int ERROR_WRITE_CONFIRM_LIST_SECTION   = 0x19;//ошибка записи списка подтверждения перезаписи
    const static int ERROR_WRITE_DATA_SECTION           = 0x20;//ошибка записи секции данных
    const static int ERROR_WRITE_COMMENT_SECTION        = 0x21;//ошибка записи секци комментариев
    const static int ERROR_CREATE_CHIPER_FILE           = 0x22;//ошибка создания  файла прошивки
    const static int ERROR_WRITE_CHIPER_FILE            = 0x23;//ошибка записи файла прошивки

    const static int SUCCESS_WRITE_FIRMWARE             = 0110;//прошивка успешно завершена
signals:
    void writeError(int code);
    void writeStatus(int code);


private:
    bool writeFileHeader();
    bool writeFWVersSection(const FWData * fwdata);
    bool writeCopyPathSection(const FWData * fwdata);
    bool writeHostName(const FWData * fwdata);
    bool writeIP_SSH(const FWData * fwdata);
    bool writePortSSH(const FWData * fwdata);
    bool writeLoginSSH(const FWData * fwdata);
    bool writePasswdSSH(const FWData * fwdata);
    bool writeConfirmLstSection(const FWData * fwdata);
    bool writeZIPSection(const FWData * fwdata);
    bool copyZipSection(FWReader * fwreader);
    bool writeCommentSection(const FWData * fwdata);
    bool chiperFW();
    TempFile tmp;//класс создания временного файла
    FILE* tmp_file;
    FILE* out_file;
    ZipCompressor zip_compr;

};

#endif // FWWRITER_H
