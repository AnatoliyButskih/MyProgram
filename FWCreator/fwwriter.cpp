#include "fwwriter.h"
#include <QFile>
#include <QByteArray>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "CommandLineParams.h"
#include <QString>
#ifndef _WIN32
#include <unistd.h>
#include "mainwindow.h"

#endif



#define CHIPER_KEY      32

#define CRC32_BUFF      4096//размер буфера вычисления контрольной суммы




#define REV_BE(w) (((w)>>24)+(((w)>>8)&0xff00)+(((w)&0xff00)<<8)+(((w)&0xff)<<24))//переворачиваем слово

using namespace std;

int m3;

void generate_chiper_key(uint8_t * key, size_t size)
{

	srand(time(NULL));//нинициализироват random
	while (size) {

		uint16_t secret = rand();

		size_t copy_size = 0;
		copy_size = size >= sizeof(uint16_t) ? sizeof(uint16_t) : size;
		memcpy(key, (uint16_t*)&secret, copy_size);
		//key+=sizeof(uint16_t);
		key += copy_size;
		size -= copy_size;
	}
}

bool FWWriter::writeFileHeader()
{
	FileHeader fheader;
	memcpy(fheader.signature, FWData::SIGNATURE, sizeof(fheader.signature));

	generate_chiper_key((uint8_t*)&fheader.gamma, sizeof(fheader.gamma));
	generate_chiper_key((uint8_t*)&fheader.key, sizeof(fheader.key));
	return fwrite(&fheader, 1, sizeof(FileHeader), tmp_file) == sizeof(FileHeader) ? true : false;
}


bool FWWriter::writeFWVersSection(const FWData * fwdata)
{

	fseek(tmp_file, 0, SEEK_END);//переход к концу файла
	uint32_t file_pos = ftell(tmp_file);//вычиление позиции в файле, она же начало секции
	SectionHeader sheader;

    //QString magic_code = CommandLineParser::instance().getValDef("-m", "new");
    //if (magic_code == "old") {
    if (m3 == 0){
		sheader.magic = FWData::START_SECTION_old;
	}
	else {
		sheader.magic = FWData::START_SECTION;
	}
	//sheader.magic=FWData::START_SECTION;
	sheader.type = FWData::VERSION_FW_T;
	sheader.crc32 = FWData::INIT_CRC32;
	sheader.offset = FWData::INIT_OFFSET;
	sheader.length = FWData::INIT_LENGTH;
	string fw_version = fwdata->getVersion();

	fw_version += '\0';//=fw_version.append("\0");//добавление нуля в конце строки версии

	if (fwrite(&sheader, 1, sizeof(SectionHeader), tmp_file) != sizeof(SectionHeader))
	{
		return false;
	}
	sheader.length = fw_version.length();
	if (sheader.length != fwrite(fw_version.c_str(), 1, fw_version.length(), tmp_file)) {
		return false;
	}

	sheader.crc32 = ZipCompressor::crc32_calc(tmp_file, file_pos + sizeof(SectionHeader), sheader.length, sheader.crc32);

	fseek(tmp_file, file_pos, SEEK_SET);
	if (fwrite(&sheader, 1, sizeof(SectionHeader), tmp_file) != sizeof(SectionHeader))
	{
		return false;
	}

	return true;
}

bool FWWriter::writeCopyPathSection(const FWData * fwdata)
{
	fseek(tmp_file, 0, SEEK_END);//переход к концу файла
	uint32_t file_pos = ftell(tmp_file);//вычиление позиции в файле, она же начало секции
	SectionHeader sheader;
	//sheader.magic=FWData::START_SECTION;
    //QString magic_code = CommandLineParser::instance().getValDef("-m", "new");
    //if (magic_code == "old") {
    if (m3 == 0){
		sheader.magic = FWData::START_SECTION_old;
	}
	else {
		sheader.magic = FWData::START_SECTION;
	}
	sheader.type = FWData::COPY_PATH_T;
	sheader.crc32 = FWData::INIT_CRC32;
	sheader.offset = FWData::INIT_OFFSET;
	sheader.length = FWData::INIT_LENGTH;
	string cp_path = fwdata->getPathOnServer();

	cp_path += '\0';//добавление нуля в конце строки


	if (fwrite(&sheader, 1, sizeof(SectionHeader), tmp_file) != sizeof(SectionHeader))
	{
		return false;
	}

	sheader.length = cp_path.length();
	if (sheader.length != fwrite(cp_path.c_str(), 1, cp_path.length(), tmp_file)) {
		return false;
	}

	sheader.crc32 = ZipCompressor::crc32_calc(tmp_file, file_pos + sizeof(SectionHeader), sheader.length, sheader.crc32);

	fseek(tmp_file, file_pos, SEEK_SET);

	if (fwrite(&sheader, 1, sizeof(SectionHeader), tmp_file) != sizeof(SectionHeader))
	{
		return false;
	}

	return true;
}
bool FWWriter::writeHostName(const FWData *fwdata)
{
	fseek(tmp_file, 0, SEEK_END);//переход к концу файла
	uint32_t file_pos = ftell(tmp_file);//вычиление позиции в файле, она же начало секции
	SectionHeader sheader;
	//sheader.magic=FWData::START_SECTION;
    //QString magic_code = CommandLineParser::instance().getValDef("-m", "new");
    //if (magic_code == "old") {
    int n =m3;
    if (m3 == 0){
		sheader.magic = FWData::START_SECTION_old;
	}
	else {
		sheader.magic = FWData::START_SECTION;
	}
	sheader.type = FWData::HOST_NAME_T;
	sheader.crc32 = FWData::INIT_CRC32;
	sheader.offset = FWData::INIT_OFFSET;
	sheader.length = FWData::INIT_LENGTH;
	string host_name = fwdata->getHostName();

	host_name += '\0';//добавление нуля в конце строки

	if (fwrite(&sheader, 1, sizeof(SectionHeader), tmp_file) != sizeof(SectionHeader))
	{
		return false;
	}

	sheader.length = host_name.length();
	if (sheader.length != fwrite(host_name.c_str(), 1, host_name.length(), tmp_file)) {
		return false;
	}

	sheader.crc32 = ZipCompressor::crc32_calc(tmp_file, file_pos + sizeof(SectionHeader), sheader.length, sheader.crc32);

	fseek(tmp_file, file_pos, SEEK_SET);

	if (fwrite(&sheader, 1, sizeof(SectionHeader), tmp_file) != sizeof(SectionHeader))
	{
		return false;
	}

	return true;
}

bool FWWriter::writeIP_SSH(const FWData *fwdata) {
	fseek(tmp_file, 0, SEEK_END);//переход к концу файла
	uint32_t file_pos = ftell(tmp_file);//вычиление позиции в файле, она же начало секции
	SectionHeader sheader;
	//sheader.magic=FWData::START_SECTION;
    //QString magic_code = CommandLineParser::instance().getValDef("-m", "new");
    //if (magic_code == "old") {
    if (m3 == 0){
		sheader.magic = FWData::START_SECTION_old;
	}
	else {
		sheader.magic = FWData::START_SECTION;
	}
	sheader.type = FWData::SSH_IP_T;
	sheader.crc32 = FWData::INIT_CRC32;
	sheader.offset = FWData::INIT_OFFSET;
	sheader.length = FWData::INIT_LENGTH;
	vector<string> ssh_ip = fwdata->getIP_SSH();
	vector<string>::iterator ssh_ip_iter = ssh_ip.begin();
	size_t need_size;

	if (fwrite(&sheader, 1, sizeof(SectionHeader), tmp_file) != sizeof(SectionHeader))
	{
		return false;
	}

	while (ssh_ip_iter != ssh_ip.end()) {
		string ip = *ssh_ip_iter;
		ip += '\0';
		need_size = ip.length();
		if (fwrite(ip.c_str(), 1, ip.length(), tmp_file) != need_size) {
			return false;
		}
		sheader.length += need_size;
		ssh_ip_iter++;
	}

	sheader.crc32 = ZipCompressor::crc32_calc(tmp_file, file_pos + sizeof(SectionHeader), sheader.length, sheader.crc32);

	fseek(tmp_file, file_pos, SEEK_SET);
	if (fwrite(&sheader, 1, sizeof(SectionHeader), tmp_file) != sizeof(SectionHeader))
	{
		return false;
	}
	return true;
}

bool FWWriter::writePortSSH(const FWData *fwdata) {
	fseek(tmp_file, 0, SEEK_END);//переход к концу файла
	uint32_t file_pos = ftell(tmp_file);//вычиление позиции в файле, она же начало секции
	SectionHeader sheader;
	//sheader.magic=FWData::START_SECTION;
    //QString magic_code = CommandLineParser::instance().getValDef("-m", "new");
    //if (magic_code == "old") {
    if (m3 == 0){
		sheader.magic = FWData::START_SECTION_old;
	}
	else {
		sheader.magic = FWData::START_SECTION;
	}
	sheader.type = FWData::SSH_PORT_T;
	sheader.crc32 = FWData::INIT_CRC32;
	sheader.offset = FWData::INIT_OFFSET;
	sheader.length = FWData::INIT_LENGTH;
	uint16_t ssh_port = fwdata->getPortSSH();

	if (fwrite(&sheader, 1, sizeof(SectionHeader), tmp_file) != sizeof(SectionHeader))
	{
		return false;
	}

	sheader.length = sizeof(ssh_port);
	if (sheader.length != fwrite(&ssh_port, 1, sizeof(ssh_port), tmp_file))
	{
		return false;
	}

	sheader.crc32 = ZipCompressor::crc32_calc(tmp_file, file_pos + sizeof(SectionHeader), sheader.length, sheader.crc32);

	fseek(tmp_file, file_pos, SEEK_SET);
	if (fwrite(&sheader, 1, sizeof(SectionHeader), tmp_file) != sizeof(SectionHeader))
	{
		return false;
	}
	return true;
}

bool FWWriter::writeLoginSSH(const FWData *fwdata)
{
	fseek(tmp_file, 0, SEEK_END);//переход к концу файла
	uint32_t file_pos = ftell(tmp_file);//вычиление позиции в файле, она же начало секции
	SectionHeader sheader;
	// sheader.magic=FWData::START_SECTION;
    //QString magic_code = CommandLineParser::instance().getValDef("-m", "new");
    //if (magic_code == "old") {
    if (m3 == 0){
		sheader.magic = FWData::START_SECTION_old;
	}
	else {
		sheader.magic = FWData::START_SECTION;
	}
	sheader.type = FWData::SSH_LOG_T;
	sheader.crc32 = FWData::INIT_CRC32;
	sheader.offset = FWData::INIT_OFFSET;
	sheader.length = FWData::INIT_LENGTH;
	string ssh_login = fwdata->getLoginSSH();

	ssh_login += '\0';//добавление нуля в конце строки

	if (fwrite(&sheader, 1, sizeof(SectionHeader), tmp_file) != sizeof(SectionHeader))
	{
		return false;
	}

	sheader.length = ssh_login.length();

	if (sheader.length != fwrite(ssh_login.c_str(), 1, ssh_login.length(), tmp_file)) {
		return false;
	}

	sheader.crc32 = ZipCompressor::crc32_calc(tmp_file, file_pos + sizeof(SectionHeader), sheader.length, sheader.crc32);

	fseek(tmp_file, file_pos, SEEK_SET);
	if (fwrite(&sheader, 1, sizeof(SectionHeader), tmp_file) != sizeof(SectionHeader))
	{
		return false;
	}

	return true;
}
bool FWWriter::writePasswdSSH(const FWData *fwdata)
{
	fseek(tmp_file, 0, SEEK_END);//переход к концу файла
	uint32_t file_pos = ftell(tmp_file);//вычиление позиции в файле, она же начало секции
	SectionHeader sheader;
	//sheader.magic=FWData::START_SECTION;
    //QString magic_code = CommandLineParser::instance().getValDef("-m", "new");
    //if (magic_code == "old") {
    if (m3 == 0){
		sheader.magic = FWData::START_SECTION_old;
	}
	else {
		sheader.magic = FWData::START_SECTION;
	}
	sheader.type = FWData::SSH_PASS_T;
	sheader.crc32 = FWData::INIT_CRC32;
	sheader.offset = FWData::INIT_OFFSET;
	sheader.length = FWData::INIT_LENGTH;
	string ssh_pass = fwdata->getPasswdSSH();

	ssh_pass += '\0';//добавление нуля в конце строки

	if (fwrite(&sheader, 1, sizeof(SectionHeader), tmp_file) != sizeof(SectionHeader))
	{
		return false;
	}
	sheader.length = ssh_pass.length();
	if (sheader.length != fwrite(ssh_pass.c_str(), 1, ssh_pass.length(), tmp_file)) {
		return false;
	}
	sheader.crc32 = ZipCompressor::crc32_calc(tmp_file, file_pos + sizeof(SectionHeader), sheader.length, sheader.crc32);
	fseek(tmp_file, file_pos, SEEK_SET);
	if (fwrite(&sheader, 1, sizeof(SectionHeader), tmp_file) != sizeof(SectionHeader))
	{
		return false;
	}
	return true;
}

bool FWWriter::writeConfirmLstSection(const FWData * fwdata)
{
	fseek(tmp_file, 0, SEEK_END);//переход к концу файла
	uint32_t file_pos = ftell(tmp_file);//вычиление позиции в файле, она же начало секции
	SectionHeader sheader;
	//sheader.magic=FWData::START_SECTION;
    //QString magic_code = CommandLineParser::instance().getValDef("-m", "new");
    //if (magic_code == "old") {
    if (m3 == 0){
		sheader.magic = FWData::START_SECTION_old;
	}
	else {
		sheader.magic = FWData::START_SECTION;
	}
	sheader.type = FWData::CONFIRM_LST_T;
	sheader.crc32 = FWData::INIT_CRC32;
	sheader.offset = FWData::INIT_OFFSET;
	sheader.length = FWData::INIT_LENGTH;
	if (fwrite(&sheader, 1, sizeof(SectionHeader), tmp_file) != sizeof(SectionHeader))
	{
		return false;
	}
	vector<string> confirm_list = fwdata->getConfirmList();
	vector<string>::iterator str_iter = confirm_list.begin();
	size_t need_size;

	while (str_iter != confirm_list.end())
	{
		string file_path = *str_iter;
		file_path += '\0';
		need_size = file_path.length();
		if (fwrite(file_path.c_str(), 1, file_path.length(), tmp_file) != need_size) {
			return false;
		}
		sheader.length += need_size;
		str_iter++;
	}
	sheader.crc32 = ZipCompressor::crc32_calc(tmp_file, file_pos + sizeof(SectionHeader), sheader.length, sheader.crc32);


	fseek(tmp_file, file_pos, SEEK_SET);
	if (fwrite(&sheader, 1, sizeof(SectionHeader), tmp_file) != sizeof(SectionHeader))
	{
		return false;
	}
	return true;
}

bool FWWriter::writeZIPSection(const FWData * fwdata)
{

	fseek(tmp_file, 0, SEEK_END);//переход к концу файла
	uint32_t file_pos = ftell(tmp_file);
	SectionHeader sheader;
	//sheader.magic=FWData::START_SECTION;
    //QString magic_code = CommandLineParser::instance().getValDef("-m", "new");
    //if (magic_code == "old") {
    if (m3 == 0){
		sheader.magic = FWData::START_SECTION_old;
	}
	else {
		sheader.magic = FWData::START_SECTION;
	}
	sheader.type = FWData::DATA_ZIP_T;
	sheader.crc32 = FWData::INIT_CRC32;
	sheader.length = FWData::INIT_LENGTH;
	if (fwrite(&sheader, 1, sizeof(SectionHeader), tmp_file) != sizeof(SectionHeader))
	{
		return false;
	}
	QFile unzip_res(":/binary/unzip");
	unzip_res.open(QIODevice::ReadOnly);
	QByteArray unzip_data = unzip_res.readAll();
	unzip_res.close();
	sheader.offset = unzip_data.size();
	if (fwrite(unzip_data.data(), 1, unzip_data.size(), tmp_file) != (size_t)unzip_data.size()) {
		return false;
	}

	if (zip_compr.zip_compess(fwdata->getLoadPath(), tmp_file)) {
		return false;
	}
	fseek(tmp_file, 0, SEEK_END);
	sheader.length = ftell(tmp_file) - file_pos - sizeof(SectionHeader);
	sheader.crc32 = ZipCompressor::crc32_calc(tmp_file, file_pos + sizeof(SectionHeader), sheader.length, sheader.crc32);
	fseek(tmp_file, file_pos, SEEK_SET);
	if (fwrite(&sheader, 1, sizeof(SectionHeader), tmp_file) != sizeof(SectionHeader))
	{
		return false;
	}
	return true;
}

bool FWWriter::copyZipSection(FWReader *fwreader) {
	FILE * file = fwreader->getFileFW();
	if (file == NULL) {
		return false;
	}
	uint32_t zip_sec_offset = fwreader->getSectionOffset(FWData::DATA_ZIP_T);
	const SectionHeader * sheader = fwreader->getSectionHeader(FWData::DATA_ZIP_T);
	if (zip_sec_offset == 0xFFFFFFFF || sheader == NULL) {
		return false;
	}

	zip_sec_offset -= sizeof(SectionHeader);//учитывем смещение заголовка
	fseek(tmp_file, 0, SEEK_END);//переход к концу файла записи
	fseek(file, zip_sec_offset, SEEK_SET);//переход к началу секции чтения
	size_t copy_size = sheader->length + sizeof(SectionHeader);
	char * copy_buff = new char[FILE_BUFF];
	size_t already_copy = 0;
	while (already_copy<copy_size) {
		size_t read_size = already_copy + FILE_BUFF>copy_size ? copy_size - already_copy : FILE_BUFF;
		read_size = fread(copy_buff, 1, read_size, file);
		if (fwrite(copy_buff, 1, read_size, tmp_file) != read_size) {
			return false;
		}
		already_copy += read_size;
	}
	return true;

}

bool FWWriter::writeCommentSection(const FWData * fwdata)
{

	fseek(tmp_file, 0, SEEK_END);//переход к концу файла
	uint32_t file_pos = ftell(tmp_file);//вычиление позиции в файле, она же начало секции
	SectionHeader sheader;
	//sheader.magic=FWData::START_SECTION;
    //QString magic_code = CommandLineParser::instance().getValDef("-m", "new");
    //if (magic_code == "old") {
    if (m3 == 0){
		sheader.magic = FWData::START_SECTION_old;
	}
	else {
		sheader.magic = FWData::START_SECTION;
	}
	sheader.type = FWData::COMMENT_FW_T;
	sheader.crc32 = FWData::INIT_CRC32;
	sheader.offset = FWData::INIT_OFFSET;
	sheader.length = FWData::INIT_LENGTH;
	string comment = fwdata->getComment();

	comment += '\0';//добавление нуля в конце строки версии

	if (fwrite(&sheader, 1, sizeof(SectionHeader), tmp_file) != sizeof(SectionHeader))
	{
		return false;
	}

	sheader.length = comment.length();
	if (fwrite(comment.c_str(), 1, comment.length(), tmp_file) != sheader.length) {
		return false;
	}
	sheader.crc32 = ZipCompressor::crc32_calc(tmp_file, file_pos + sizeof(SectionHeader), sheader.length, sheader.crc32);
	fseek(tmp_file, file_pos, SEEK_SET);
	if (fwrite(&sheader, 1, sizeof(SectionHeader), tmp_file) != sizeof(SectionHeader))
	{
		return false;
	}
	return true;
}


FWWriter::FWWriter() {

}

int FWWriter::write_firmware(const FWData *fwdata, FWReader *fwreader) {
	tmp_file = tmp.create();
	if (tmp_file == NULL) {
		emit writeError(ERROR_CREATE_TMP_FILE);
		return -1;
	}

	if (!writeFileHeader()) {
		tmp.destroy();
		emit writeError(ERROR_WRITE_FILE_HEADER);
		return -1;
	}

	if (!writeFWVersSection(fwdata))//записали версию файла
	{
		tmp.destroy();
		emit writeError(ERROR_WRITE_VERSION_SECTION);
		return -1;
	}
	if (!writeCopyPathSection(fwdata))//записали  путь копирования на сервер
	{
		tmp.destroy();
		emit writeError(ERROR_WRITE_PATH_SECTION);
		return -1;
	}
	if (!writeHostName(fwdata))
	{
		tmp.destroy();
		emit writeError(ERROR_WRITE_HOSTNAME_SECTION);
		return -1;
	}

	if (!writeIP_SSH(fwdata)) {
		tmp.destroy();
		emit writeError(ERROR_WRITE_SSH_IP_SECTION);
		return -1;
	}

	if (!writePortSSH(fwdata)) {
		tmp.destroy();
		emit writeError(ERROR_WRITE_SSH_PORT_SECTION);
		return -1;
	}

	if (!writeLoginSSH(fwdata))//записали  логин SSH
	{
		tmp.destroy();
		emit writeError(ERROR_WRITE_SSH_LOGIN_SECTION);
		return -1;
	}

	if (!writePasswdSSH(fwdata))//записали  пароль SSH
	{
		tmp.destroy();
		emit writeError(ERROR_WRITE_SSH_PASSWD_SECTION);
		return -1;
	}

	if (!writeConfirmLstSection(fwdata))//записали список файлов требующих подтверждения
	{
		tmp.destroy();
		emit writeError(ERROR_WRITE_CONFIRM_LIST_SECTION);
		return -1;
	}
	if (fwreader->isInit()) {//проверяем что в данный момент в программе данные из другой прошивки
		if (!copyZipSection(fwreader))//записали Zip секцию файла
		{
			tmp.destroy();
			emit writeError(ERROR_WRITE_DATA_SECTION);
			return -1;
		}
	}
	else {
		if (!writeZIPSection(fwdata))//записали список файлов требующих подтверждения
		{
			tmp.destroy();
			emit writeError(ERROR_WRITE_DATA_SECTION);
			return -1;
		}
	}
	if (!writeCommentSection(fwdata)) {//комментирующая секция
		tmp.destroy();
		emit writeError(ERROR_WRITE_COMMENT_SECTION);
		return -1;
	}
	out_file = fopen(fwdata->getOutFWName().c_str(), "wb+");
	if (out_file == NULL) {
		tmp.destroy();
		emit writeError(ERROR_CREATE_CHIPER_FILE);
		return -1;
	}
	if (!chiperFW()) {//шифрование файла
		tmp.destroy();
		fclose(out_file);
		remove(fwdata->getOutFWName().c_str());//удаляем неудачный файл
		emit writeError(ERROR_WRITE_CHIPER_FILE);
		return -1;
	}
	tmp.destroy();
	fclose(out_file);
	emit writeStatus(SUCCESS_WRITE_FIRMWARE);
	return 0;
}

bool FWWriter::chiperFW()
{

	fseek(out_file, 0, SEEK_SET);//переход к началу файла
	fseek(tmp_file, 0, SEEK_SET);//переход к началу файла
	FileHeader fheader;
	fread(&fheader, 1, sizeof(FileHeader), tmp_file);
	fwrite(&fheader, 1, sizeof(FileHeader), out_file);
	GhostChiper ghost_chiper(fheader.key);
	size_t read_size = 0;
	char * read_buff = new char[FILE_BUFF];
	while ((read_size = fread(read_buff, 1, FILE_BUFF, tmp_file)) != 0) {
		fheader.gamma = ghost_chiper.ghost_gamma_codec(fheader.gamma, read_buff, read_size, true);
		if (fwrite(read_buff, 1, read_size, out_file) != read_size) {
			return false;
		}
	}
	delete[]read_buff;
	return true;
}
