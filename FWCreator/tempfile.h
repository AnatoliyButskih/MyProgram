#ifndef TEMPFILE_H
#define TEMPFILE_H
#include <string>
#include <stdio.h>

using namespace std;
class TempFile
{
  
public:
    TempFile();
    ~TempFile();
    FILE * create();
    void destroy();
private:
    FILE * tmp_file;
    string file_path;


};

#endif // TEMPFILE_H
