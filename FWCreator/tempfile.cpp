#include "tempfile.h"

#ifdef _WIN32
#include <windows.h>
#endif
TempFile::TempFile()
{
    file_path.clear();
    tmp_file=NULL;
}
TempFile::~TempFile(){
    destroy();
}

FILE * TempFile::create()
{
#ifdef _WIN32
char path_buffer[4096];
int length=GetEnvironmentVariableA("TEMP",path_buffer,4096);

if(length==0 && GetLastError()==ERROR_ENVVAR_NOT_FOUND){//если не найдена переменная TEMP ищем TMP
    length=GetEnvironmentVariableA("TMP",path_buffer,4096);
}

if(length==0){
    return NULL;
}

char * tmp_file_name=_tempnam(path_buffer,"user");

if(tmp_file_name==NULL){
     return NULL;
}

file_path=string(tmp_file_name);
tmp_file=fopen(file_path.c_str(),"wb+");
if(tmp_file==NULL){
	file_path.clear();
}
return tmp_file;
#else
tmp_file=tmpfile();
return tmp_file;
#endif

}

void TempFile::destroy(){
    if(tmp_file!=NULL){
        fclose(tmp_file);
        tmp_file=NULL;
    }
    #ifdef _WIN32
	if(!file_path.empty()){
		remove(file_path.c_str());
	}
    #endif
}
