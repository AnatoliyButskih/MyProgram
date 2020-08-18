#include "fwreader.h"
#include "ghostchiper.h"
#include <string>
#include "CommandLineParams.h"

#define CRC32_BUFF      4096//размер буфера вычисления контрольной суммы



size_t fw_readline(FILE *file,char delim, QString &str){
    char buff;
    size_t read_size=0;
    size_t cur_read;
    string stl_str;
    while((cur_read=fread(&buff,1,sizeof(buff),file)!=0)){
        read_size+=cur_read;
        if(buff!=delim){
            stl_str+=buff;
        }else{
            break;
        }
    }
    if(stl_str.empty()){
        str=QString();
    }else{
        str=QString::fromUtf8(stl_str.c_str());
    }
    return read_size;
}



FWReader::FWReader()
{
    in_file=NULL;
    init_status=false;
    zip_compressor=new ZipCompressor;
    zip_files=NULL;
}
FWReader::~FWReader(){
    clear();
    delete zip_compressor;
}

int FWReader::init_read(QString file_name){
    clear();
    FILE * chipre_file=fopen(file_name.toLocal8Bit(),"rb");
    if(chipre_file==NULL){
        return OPEN_ERROR;
    }
    in_file=plainFW(chipre_file);
    if(in_file==NULL){
        return OPEN_ERROR;
    }
    int ret;
    fseek(in_file, 0 , SEEK_SET);//переход к началу файла
    FileHeader fheader;

    if(fread(&fheader,1,sizeof(FileHeader),in_file)!=sizeof(FileHeader)){
        return SIGNATURE_READ_ERROR;
    }

    ret=strncmp(fheader.signature,FWData::SIGNATURE,SIGNATURE_SIZE);
    if(ret!=0){
        return SIGNATURE_READ_ERROR;
    }
    SectionHeader sheader;
    while(fread(&sheader,1,sizeof(SectionHeader),in_file)!=0){
        if(sheader.magic==FWData::START_SECTION){
            uint32_t sec_offset=ftell(in_file);
            uint32_t crc32=0xFFFFFFFF;
            crc32=ZipCompressor::crc32_calc(in_file,sec_offset, sheader.length,crc32);
            if(crc32!=sheader.crc32)
            {
                return CRC32_READ_ERROR;
            }
            sections.insert(pair<uint32_t,SectionHeader>(sec_offset,sheader));
            fseek(in_file,sec_offset+sheader.length,SEEK_SET);
        }else {
            if(sheader.magic==FWData::START_SECTION_old){
                uint32_t sec_offset=ftell(in_file);
                uint32_t crc32=0xFFFFFFFF;
                crc32=ZipCompressor::crc32_calc(in_file,sec_offset, sheader.length,crc32);
                if(crc32!=sheader.crc32)
                {
                    return CRC32_READ_ERROR;
                }
                sections.insert(pair<uint32_t,SectionHeader>(sec_offset,sheader));
                fseek(in_file,sec_offset+sheader.length,SEEK_SET);
            }else {
                return SECTION_ERROR;
            }
        }

    }

    a = "undefined";
    QString a1= QString::number(sheader.magic);
   // if(a1 == "609887322") {
    if(a1 == "609952859") {
        a = "0x245B245B";
    } else {
        a = "0x245B245A";
    }

    init_status=true;
    return NO_READ_ERROR;
}

bool FWReader::isInit()
{
    return init_status;
}


uint32_t FWReader::getSectionOffset(uint16_t section_type){
    map<uint32_t,SectionHeader>::iterator iter=sections.begin();
    while(iter!=sections.end()){
        if(iter->second.type==section_type){
            return iter->first;
        }
        iter++;
    }
    return 0xFFFFFFFF;
}
const SectionHeader * FWReader::getSectionHeader(uint16_t section_type){
    map<uint32_t,SectionHeader>::iterator iter=sections.begin();
    while(iter!=sections.end()){
        if(iter->second.type==section_type){
            return &iter->second;
        }
        iter++;
    }
    return NULL;
}

ZipFileItem * FWReader::getZipFileStructure(){
    uint32_t zip_sec_offset=getSectionOffset(FWData::DATA_ZIP_T);
    const SectionHeader * sheader=getSectionHeader(FWData::DATA_ZIP_T);
    if(zip_sec_offset==0xFFFFFFFF || sheader==NULL){
        return NULL;
    }
    return zip_compressor->zip_files(in_file,zip_sec_offset+sheader->offset,sheader->length-sheader->offset);
}
QString FWReader::getVersionFW()
{
    uint32_t zip_sec_offset=getSectionOffset(FWData::VERSION_FW_T);
    const SectionHeader * sheader=getSectionHeader(FWData::VERSION_FW_T);
    if(zip_sec_offset==0xFFFFFFFF || sheader==NULL){
        emit readError(ERROR_GET_VERSION_FW);
        return QString();
    }
    fseek(in_file,zip_sec_offset,SEEK_SET);

    QString fw_version;
    fw_readline(in_file,0,fw_version);

    return fw_version;
}
QString FWReader::getPathOnServer(){
    uint32_t zip_sec_offset=getSectionOffset(FWData::COPY_PATH_T);
    const SectionHeader * sheader=getSectionHeader(FWData::COPY_PATH_T);
    if(zip_sec_offset==0xFFFFFFFF || sheader==NULL){
        emit readError(ERROR_GET_PATH_ON_SERVER);
        return QString();
    }
    fseek(in_file,zip_sec_offset,SEEK_SET);
    QString path_on_server;
    fw_readline(in_file,0,path_on_server);
    return path_on_server;
}

QString FWReader::getHostName()
{
    uint32_t zip_sec_offset=getSectionOffset(FWData::HOST_NAME_T);
    const SectionHeader * sheader=getSectionHeader(FWData::HOST_NAME_T);
    if(zip_sec_offset==0xFFFFFFFF || sheader==NULL){
        emit readError(ERROR_GET_HOSTNAME);
        return QString();
    }
    fseek(in_file,zip_sec_offset,SEEK_SET);
    QString host_name;
    fw_readline(in_file,0,host_name);
    return host_name;
}

QStringList FWReader::getIP_SSH(){
    QStringList ip_list;
    uint32_t zip_sec_offset=getSectionOffset(FWData::SSH_IP_T);
    const SectionHeader * sheader=getSectionHeader(FWData::SSH_IP_T);
    if(zip_sec_offset==0xFFFFFFFF || sheader==NULL){
        emit readError(ERROR_GET_SSH_IP);
        return ip_list;
    }
    fseek(in_file,zip_sec_offset,SEEK_SET);
    size_t read_size=0;
    fseek(in_file,zip_sec_offset,SEEK_SET);
    while(read_size<sheader->length)
    {
        QString ip;
        read_size+=fw_readline(in_file,0,ip);
        ip_list.push_back(ip);
    }
    return ip_list;
}

uint16_t FWReader::getPortSSH(){
    uint32_t zip_sec_offset=getSectionOffset(FWData::SSH_PORT_T);
    const SectionHeader * sheader=getSectionHeader(FWData::SSH_PORT_T);
    if(zip_sec_offset==0xFFFFFFFF || sheader==NULL){
        emit readError(ERROR_GET_SSH_PORT);
        return 0;
    }
    fseek(in_file,zip_sec_offset,SEEK_SET);
    uint16_t ssh_port;
    if(fread(&ssh_port,1,sizeof(ssh_port),in_file)!=sizeof(ssh_port)){
        return 0;
    }
    return ssh_port;
}

QString FWReader::getLoginSSH(){
    uint32_t zip_sec_offset=getSectionOffset(FWData::SSH_LOG_T);
    const SectionHeader * sheader=getSectionHeader(FWData::SSH_LOG_T);
    if(zip_sec_offset==0xFFFFFFFF || sheader==NULL){
        emit readError(ERROR_GET_SSH_LOGIN);
        return QString();
    }
    fseek(in_file,zip_sec_offset,SEEK_SET);
    QString ssh_login;
    fw_readline(in_file,0,ssh_login);
    return ssh_login;
}

QString FWReader::getPasswdSSH(){
    uint32_t zip_sec_offset=getSectionOffset(FWData::SSH_PASS_T);
    const SectionHeader * sheader=getSectionHeader(FWData::SSH_PASS_T);
    if(zip_sec_offset==0xFFFFFFFF || sheader==NULL){
        emit readError(ERROR_GET_SSH_PASSWORD);
        return QString();
    }
    fseek(in_file,zip_sec_offset,SEEK_SET);
    QString ssh_passwd;
    fw_readline(in_file,0,ssh_passwd);
    return ssh_passwd;
}

QString FWReader::getComment()
{
    uint32_t zip_sec_offset=getSectionOffset(FWData::COMMENT_FW_T);
    const SectionHeader * sheader=getSectionHeader(FWData::COMMENT_FW_T);
    if(zip_sec_offset==0xFFFFFFFF || sheader==NULL){
        emit readError(ERROR_GET_COMMENT);
        return QString();
    }
    fseek(in_file,zip_sec_offset,SEEK_SET);
    QString comment;
    fw_readline(in_file,0,comment);
    return comment;
}

QStringList FWReader::getConfirmList(){
    QList<QString> confirm_list;
    uint32_t zip_sec_offset=getSectionOffset(FWData::CONFIRM_LST_T);
    const SectionHeader * sheader=getSectionHeader(FWData::CONFIRM_LST_T);
    if(zip_sec_offset==0xFFFFFFFF || sheader==NULL){
        emit readError(ERROR_GET_CONFIRM_LIST);
        return confirm_list;
    }
    size_t read_size=0;
    fseek(in_file,zip_sec_offset,SEEK_SET);
    while(read_size<sheader->length)
    {
        QString confirm_file;
        read_size+=fw_readline(in_file,0,confirm_file);
        confirm_list.push_back(confirm_file);
    }
    return confirm_list;
}

int FWReader::extractFW(QString extract_path)
{
    uint32_t zip_sec_offset=getSectionOffset(FWData::DATA_ZIP_T);
    const SectionHeader * sheader=getSectionHeader(FWData::DATA_ZIP_T);
    if(zip_sec_offset==0xFFFFFFFF || sheader==NULL){
        emit readError(ERROR_EXTRACT_DATA);
        return -1;
    }
    string out_folder(extract_path.toLocal8Bit().data());
    if(zip_compressor->zip_decompess(out_folder,in_file,zip_sec_offset+sheader->offset,sheader->length-sheader->offset)){
        emit readError(ERROR_EXTRACT_DATA);
        return -1;
    }
    QString file_name=extract_path+"/infotrans_version_fw.txt";
    FILE * file = fopen(file_name.toUtf8(),"w");
    if(file==NULL){
        emit readError(ERROR_EXTRACT_DATA);
        return -1;
    }
    QByteArray write_data;
    size_t write_size;
    QString version_fw("version=");
    version_fw.append(getVersionFW()+"\n");
    write_data=version_fw.toUtf8();
    write_size=(size_t)write_data.length();
    if(fwrite (write_data.data(), 1, write_size, file)!=write_size){
        emit readError(ERROR_EXTRACT_DATA);
        fclose(file);
        return -1;
    }

    QString path_on_controller("path_on_controller=");
    path_on_controller.append(getPathOnServer()+"\n");
    write_data=path_on_controller.toUtf8();
    write_size=(size_t)write_data.length();
    if(fwrite (write_data.data(), 1, write_size, file)!=write_size){
        emit readError(ERROR_EXTRACT_DATA);
        fclose(file);
        return -1;
    }

    QString host_name("host_name=");
    host_name.append(getHostName()+"\n");
    write_data=host_name.toUtf8();
    write_size=(size_t)write_data.length();
    if(fwrite (write_data.data(), 1, write_size, file)!=write_size){
        emit readError(ERROR_EXTRACT_DATA);
        fclose(file);
        return -1;
    }

    //    QString ssh_ip("ssh ip=");
    //    ssh_ip.append(getIP_SSH()+"\n");
    //    write_data=ssh_ip.toUtf8();
    //    write_size=(size_t)write_data.length();
    //    if(fwrite (write_data.data(), 1, write_size, file)!=write_size){
    //        emit readError(ERROR_EXTRACT_DATA);
    //        fclose(file);
    //        return -1;
    //    }

    QString ssh_ip_list("\nSSH IP list:\n");
    write_data=ssh_ip_list.toUtf8();
    write_size=(size_t)write_data.length();
    if(fwrite (write_data.data(), 1, write_size, file)!=write_size){
        emit readError(ERROR_EXTRACT_DATA);
        fclose(file);
        return -1;
    }

    QStringList ssh_ip=getIP_SSH();
    for(int i=0;i<ssh_ip.size();i++){
        QString ip=ssh_ip.at(i);
        ip.append("\n");
        write_data=ip.toUtf8();
        write_size=(size_t)write_data.length();
        if(fwrite (write_data.data(), 1, write_size, file)!=write_size){
            emit readError(ERROR_EXTRACT_DATA);
            fclose(file);
            return -1;
        }
    }

    QString ssh_port("ssh_port=");
    ssh_port.append(QString::number(getPortSSH())+"\n");
    write_data=ssh_port.toUtf8();
    write_size=(size_t)write_data.length();
    if(fwrite (write_data.data(), 1, write_size, file)!=write_size){
        emit readError(ERROR_EXTRACT_DATA);
        fclose(file);
        return -1;
    }

    QString ssh_login("ssh_login=");
    ssh_login.append(getLoginSSH()+"\n");
    write_data=ssh_login.toUtf8();
    write_size=(size_t)write_data.length();
    if(fwrite (write_data.data(), 1, write_size, file)!=write_size){
        emit readError(ERROR_EXTRACT_DATA);
        fclose(file);
        return -1;
    }

    QString ssh_passwd("ssh_passwd=");
    ssh_passwd.append(getPasswdSSH()+"\n");
    write_data=ssh_passwd.toUtf8();
    write_size=(size_t)write_data.length();
    if(fwrite (write_data.data(), 1, write_size, file)!=write_size){
        emit readError(ERROR_EXTRACT_DATA);
        fclose(file);
        return -1;
    }

    QString confirm_list("\nConfirm list:\n");
    write_data=confirm_list.toUtf8();
    write_size=(size_t)write_data.length();
    if(fwrite (write_data.data(), 1, write_size, file)!=write_size){
        emit readError(ERROR_EXTRACT_DATA);
        fclose(file);
        return -1;
    }


    QString g1 = CommandLineParser::instance().getValDef("-geometry", "0");
    QString c1 = CommandLineParser::instance().getValDef("-calibr", "0");

    if (g1=="1"){
        QString confg("config/common/geometry.cfg");
        write_data=confg.toUtf8();
        write_size=(size_t)write_data.length();
        if(fwrite (write_data.data(), 1, write_size, file)!=write_size){
            emit readError(ERROR_EXTRACT_DATA);
            fclose(file);
            return -1;
        }
    }

    if (c1=="1"){
        QString confc("config/common/calibr.cfg");
        write_data=confc.toUtf8();
        write_size=(size_t)write_data.length();
        if(fwrite (write_data.data(), 1, write_size, file)!=write_size){
            emit readError(ERROR_EXTRACT_DATA);
            fclose(file);
            return -1;
        }
    }


    QList<QString> confirm=getConfirmList();
    for(int i=0;i<confirm.size();i++){
        QString confirm_name=confirm.at(i);
        confirm_name.append("\n");
        write_data=confirm_name.toUtf8();
        write_size=(size_t)write_data.length();
        if(fwrite (write_data.data(), 1, write_size, file)!=write_size){
            emit readError(ERROR_EXTRACT_DATA);
            fclose(file);
            return -1;
        }
    }

    QString comment("\nComment:\n");
    write_data=comment.toUtf8();
    write_size=(size_t)write_data.length();
    if(fwrite (write_data.data(), 1, write_size, file)!=write_size){
        emit readError(ERROR_EXTRACT_DATA);
        fclose(file);
        return -1;
    }
    comment=getComment();
    write_data=comment.toUtf8();
    write_size=(size_t)write_data.length();
    if(fwrite (write_data.data(), 1, write_size, file)!=write_size){
        emit readError(ERROR_EXTRACT_DATA);
        fclose(file);
        return -1;
    }

    fclose(file);


    emit readStatus(SUCCESS_EXTRACT_DATA);
    return 0;
}

FILE * FWReader::getFileFW(){
    return in_file;
}

void FWReader::clear()
{
    if(in_file!=NULL){
        tmp.destroy();

    }
    in_file=NULL;
    sections.clear();//очищаем данные перед загрузкой новых данных
    init_status=false;
}

FILE* FWReader::plainFW(FILE * chiper_file){

    FILE* plain_file = tmp.create();//fopen("unchip_tmp.bin","wb+"); //
    if(plain_file==NULL || chiper_file==NULL){
        return NULL;
    }

    fseek(chiper_file,0,SEEK_SET);//переход к началу шифрованного файла
    fseek(plain_file,0,SEEK_SET);//переход к началу открытого файла
    FileHeader fheader;
    if(fread(&fheader,1,sizeof(FileHeader),chiper_file)!=sizeof(FileHeader)){
        fclose(chiper_file);
        tmp.destroy();
        return NULL;
    }
    if(fwrite(&fheader,1,sizeof(FileHeader),plain_file)!=sizeof(FileHeader)){
        fclose(chiper_file);
        tmp.destroy();
        return NULL;
    }
    GhostChiper ghost_chiper(fheader.key);
    size_t read_size=0;
    char * read_buff=new char[FILE_BUFF];
    //fheader.gamma=ghost_chiper.ghost_coding(fheader.gamma);
    while((read_size=fread(read_buff,1,FILE_BUFF,chiper_file))>0){
        fheader.gamma=ghost_chiper.ghost_gamma_codec(fheader.gamma, read_buff,read_size,false);
        if(fwrite(read_buff,1,read_size,plain_file)!=read_size){
            fclose(chiper_file);
            tmp.destroy();
            return NULL;
        }
    }
    //fflush(plain_file);
    delete []read_buff;
    fclose(chiper_file);
    return plain_file;
    //return NULL;

}

QString FWReader::getMagic(){


    QString magicfirmware;
    magicfirmware = a;
    return magicfirmware;

}
