#ifndef FWDATA_H
#define FWDATA_H
#include <string>
#include <vector>
#include <QString>
#include <QStringList>
#include "confirmlistmodel.h"

#define SIGNATURE_SIZE      9//размер начальной сигнатуры
#define CHIPER_KEY_ELEMENTS  8//количество 32-х битных элемнтов в ключе
#define CHIPER_KEY_SIZE     32//размер ключа в байтах (генерируется случайным образом)
#define GHOST_GAMMA_SIZE    8//размер гаммы ГОСТ (генерируется случайным образом)
#define FILE_BUFF           8192//размер буфера считываемого из файла

using namespace std;



#pragma pack (push,1)
struct FileHeader{
    char signature[SIGNATURE_SIZE];
    uint64_t gamma;
    uint32_t key[CHIPER_KEY_ELEMENTS];
};
#pragma pack(pop)

#pragma pack (push,1)
struct SectionHeader{
    uint32_t magic;
    uint16_t type;
    uint32_t crc32;
    uint32_t offset;//размер служебной информации в начале секции
    uint32_t length;
};
#pragma pack(pop)

class FWData//данные записываемые в прошивку
{
public:
    FWData();
    void reset();//очистить данные перед новой заливкой
    void setVersion(const QString & version);
    void setPathOnServer(const QString & path);
    void setHostName(const QString & hostname);
    void setIP_SSH(const QStringList & ip);
    void setPortSSH(int port);
    void setLoginSSH(const QString & login);
    void setPasswdSSH(const QString & passwd);
    void setConfirmList(const QList<ConfirmFile>& confirm);
    void setLoadPath(const QString & path);
    void setOutFWName(const QString & name);
    void setComment(const QString & comment);
    void addConfirmFile(const string &path);



    string getVersion() const;
    string getPathOnServer() const;
    string getHostName() const;
    vector<string> getIP_SSH() const;
    uint16_t getPortSSH() const;
    string getLoginSSH()const;
    string getPasswdSSH() const;
    string getLoadPath() const;
    string getOutFWName() const;
    string getComment()const;
    vector<string>  getConfirmList() const;



    const static char SIGNATURE[SIGNATURE_SIZE];

    const static uint32_t START_SECTION = 0x245B245B;//начало секции
    const static uint32_t START_SECTION_old = 0x245A245A;//начало секции // старые версии fwcreator
    const static uint32_t INIT_CRC32    = 0xFFFFFFFF;//начальное значение контрольной суммы
    const static uint32_t INIT_LENGTH   = 0x0;       //начальное значение длинны секции
    const static uint32_t INIT_OFFSET   = 0x0;       //начальное значение длинны секции


    const static uint16_t VERSION_FW_T  = 0x0011;   //тип секции - версия ПО
    const static uint16_t COPY_PATH_T   = 0x0012;   //тип секции - путь копирования на сервере
    const static uint16_t CONFIRM_LST_T = 0x0013;   //тип секции - содержит список файлов на подтвердение копированя
    const static uint16_t DATA_ZIP_T    = 0x0014;   //тип секции - данные прошивки в ZIP формате
    const static uint16_t COMMENT_FW_T  = 0x0015;   //тип секции - секция комментариев
    const static uint16_t SSH_LOG_T     = 0x0016;   //тип секции - секция логин SSH
    const static uint16_t SSH_PASS_T    = 0x0017;   //тип секции - секция пароль SSH
    const static uint16_t SSH_IP_T      = 0x0018;   //тип секции - секция IP адреса SSH
    const static uint16_t SSH_PORT_T    = 0x0019;   //тип секции - секция IP порт SSH
    const static uint16_t HOST_NAME_T   = 0x0020;   //тип секции - секция имени хоста контроллера


private:
    string version;//версия прошивки
    string path_on_server;//путь копирования на сервере
    string host_name;

    uint16_t ssh_port;//SSH порт на контроллере
    uint16_t magic_code1;//SSH порт на контроллере
    string ssh_login;//логин SSH для доступа к серверу
    string ssh_passwd;//пароль SSH
    string load_path;//путь загрузки прошивки с жесткого диска
    string fw_name;//путь сохранения выбранной прошивки
    string comment;//произвольный текстовый комментарий
    vector<string> confirm_list;//список содержащий файлы на подтверждение
    vector<string> ssh_ip;//IP адреса контроллеров
};

#endif // FWDATA_H
