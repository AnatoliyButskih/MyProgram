#include "fwdata.h"
#include "CommandLineParams.h"
#include <iostream>
using namespace std;

const char FWData::SIGNATURE[SIGNATURE_SIZE]={'i','n','f','o','t','r','a','n','s'};


FWData::FWData()
{
}
void FWData::reset()
{
    version.clear();//версия прошивки
    path_on_server.clear();//путь копирования на сервере
    host_name.clear();
    ssh_port=0;//SSH порт на контроллере
    ssh_login.clear();//логин SSH для доступа к серверу
    ssh_passwd.clear();//пароль SSH
    load_path.clear();//путь загрузки прошивки с жесткого диска
    fw_name.clear();//путь сохранения выбранной прошивки
    comment.clear();//произвольный текстовый комментарий
    confirm_list.clear();//список подтверждения
    ssh_ip.clear();//IP адреса хостов
}


void FWData::setVersion(const QString & version)
{
    this->version=string(version.toUtf8().data());
}

void FWData::setPathOnServer(const QString &path){
    path_on_server=string(path.toUtf8().data());
}

void FWData::setHostName(const QString &hostname){
    host_name=string(hostname.toUtf8().data());
}

void FWData::setIP_SSH(const QStringList  &ip){
    ssh_ip.clear();
    for(int i=0; i<ip.size(); i++){
        ssh_ip.push_back(string(ip.at(i).toUtf8().data()));
    }

}

void FWData::setPortSSH(int port){
    ssh_port=port;
}


void FWData::setLoginSSH(const QString &login){
    ssh_login=string(login.toUtf8().data());
}

void FWData::setPasswdSSH(const QString &passwd){
    ssh_passwd=string(passwd.toUtf8().data());
}

void FWData::setConfirmList(const QList<ConfirmFile> &confirm)
{
    confirm_list.clear();
    for(int i=0;i<confirm.size();i++){
        string confirm_file=string(confirm.at(i).filePath.toLocal8Bit().data());
        confirm_list.push_back(confirm_file);
    }
}

void FWData::setLoadPath(const QString &path)
{
    load_path=string(path.toLocal8Bit().data());
}

void FWData::setOutFWName(const QString &name)
{
    fw_name=string(name.toLocal8Bit().data());
}

void FWData::setComment(const QString &comment){
    this->comment=string(comment.toUtf8().data());
}

void FWData::addConfirmFile(const string &path)
{
   confirm_list.push_back(path);
}

string FWData::getVersion() const
{
    return version;
}

string FWData::getPathOnServer() const
{
    return path_on_server;
}

string FWData::getHostName() const
{
    return host_name;
}


vector<string> FWData::getIP_SSH() const
{
    return ssh_ip;
}

uint16_t FWData::getPortSSH() const
{
    return ssh_port;
}

string FWData::getLoginSSH() const
{
    return ssh_login;
}

string FWData::getPasswdSSH() const
{
    return ssh_passwd;
}

string FWData::getLoadPath() const
{
    return load_path;
}

string FWData::getOutFWName() const
{
    return fw_name;
}

string FWData::getComment() const
{
    return comment;
}


vector<string> FWData::getConfirmList() const
{
    return confirm_list;
}
