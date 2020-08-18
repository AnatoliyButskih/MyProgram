#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <QFileDialog>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QTime>
#include <QTranslator>
#include <QSettings>
#include <qstring.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

using namespace std;

void MainWindow::setArgs(){
    ui->path_Edit->setText("");
    ui->file_Edit->setText("DSxx.p3");
    ui->descriptor_Edit->setText("DSxx");
    ui->descriptor_Edit_2->setText("DSXX");
}

void MainWindow::fix()
{
  if (q==true){
       int offset = Offset.toInt();
       string mc = File.toStdString();
       string tName = TName.toStdString();

        uint8_t ttName[4];
       ttName[0] = (uint8_t)tName.c_str()[0];
        ttName[1] = (uint8_t)tName.c_str()[1];
       ttName[2] = (uint8_t)tName.c_str()[2];
        ttName[3] = (uint8_t)tName.c_str()[3];

       FILE *fid;
        // if((fid=fopen(mc.c_str(), "rb+"))==NULL) {}
        QString dirconfig = Offset+"/"+File;
        string last2 = dirconfig.toStdString();
        // string last1 = last.toStdString();
       if((fid=fopen(last2.c_str(), "rb+"))==NULL) {
       }


       fseek( fid , 0 , SEEK_END );
       int64_t  szf = ftell(fid);
       fseek( fid , 0 , SEEK_SET );
       fseek( fid , offset , SEEK_SET );


        int64_t nb = offset;
        int64_t st = 0;
        int16_t c;
        while(1){
            if (nb > szf-2){ break; }
            fseek( fid , nb , SEEK_SET );
            fread(&c, 1, sizeof(uint16_t), fid);       // считываем файл в буфер
           if (c == 9306){
               fseek(fid , 14 , SEEK_CUR );
                fwrite(&ttName[0], sizeof(uint8_t), 1, fid);
                fwrite(&ttName[1], sizeof(uint8_t), 1, fid);
               fwrite(&ttName[2], sizeof(uint8_t), 1, fid);
                fwrite(&ttName[3], sizeof(uint8_t), 1, fid);
               st = st+1;
           }
            nb = nb + 1;
            if(st == 2){ break; }
        }
        fclose(fid);
        QMessageBox::warning(this,tr("Finish"),tr("Successfully. Descriptor restored."));
    }
}

// Linux
/*
void MainWindow::fixm()
{

    if (q==true){
ttNameOk=2;
        int offset = Offset.toInt();
        string mc = File.toStdString();
        string tName = TName.toStdString();
        string tName_out = TNameOut.toStdString();

        uint8_t ttName_in[4];
        ttName_in[0] = (uint8_t)tName.c_str()[0];
        ttName_in[1] = (uint8_t)tName.c_str()[1];
        ttName_in[2] = (uint8_t)tName.c_str()[2];
        ttName_in[3] = (uint8_t)tName.c_str()[3];

        uint8_t ttName_read[4];
        uint8_t ttName[4];
        ttName[0] = (uint8_t)tName_out.c_str()[0];
        ttName[1] = (uint8_t)tName_out.c_str()[1];
        ttName[2] = (uint8_t)tName_out.c_str()[2];
        ttName[3] = (uint8_t)tName_out.c_str()[3];

        //FILE *fid;
        //if((fid=fopen64(mc.c_str(), "rb+"))==NULL) {}
        FILE *fid;
        QString dirconfig = Offset+"/"+File;
        string last2 = dirconfig.toStdString();
        if((fid=fopen64(last2.c_str(), "rb+"))==NULL) {
        }
        fseeko64( fid , 0 , SEEK_END );
        int64_t  szf = ftello64(fid);
        fseeko64( fid , 0 , SEEK_SET );
        fseeko64( fid , offset , SEEK_SET );
        int64_t nb = offset;
        int64_t st = 0;
        int16_t c;
        while(1){
            if (nb > szf-2){ break; }
            fseeko64( fid , nb , SEEK_SET );
            fread(&c, 1, sizeof(uint16_t), fid);       // считываем файл в буфер
            if (c == 9306){
                fseek(fid , 14 , SEEK_CUR );
                fread(ttName_read, 4, sizeof(uint8_t), fid);       // считываем файл в буфер

                if (
                        ttName_read[0] == ttName_in[0] &&
                        ttName_read[1] == ttName_in[1] &&
                        ttName_read[2] == ttName_in[2] &&
                        ttName_read[3] == ttName_in[3] )
                {
                    fseeko64(fid , -4, SEEK_CUR );
                    fwrite(&ttName[0], sizeof(uint8_t), 1, fid);
                    fwrite(&ttName[1], sizeof(uint8_t), 1, fid);
                    fwrite(&ttName[2], sizeof(uint8_t), 1, fid);
                    fwrite(&ttName[3], sizeof(uint8_t), 1, fid);
                    st = st+1;
                }

            }
            nb = nb + 1;
            if(st == 2){ break; }
        }

        if (ttName_read[0] == ttName_in[0] &&
            ttName_read[1] == ttName_in[1] &&
            ttName_read[2] == ttName_in[2] &&
            ttName_read[3] == ttName_in[3])
       {
            QMessageBox::warning(this,tr("Finish"),tr("Successfully. Descriptor renamed."));
       } else
       {
            QMessageBox::warning(this,tr("Error"),tr("Mistake. The name of the source descriptor is incorrect. The name of the buffer descriptor will not be renamed."));
        }
        fclose(fid);
        //QMessageBox::warning(this,tr("Finish"),tr("Successfully. Descriptor renamed."));
    }
}
*/

void MainWindow::aboutinfo()
{

    QMessageBox::about(this,tr("About FixP3 2020-06-21"), tr("<h3>FixP3</h3> <p>Developer: Rinat Diyazitdinov MTD</p> <p>Developer: Anatoliy Butskikh DPR</p> <p>Restore - restores the buffer descriptor</p> <p>Filled fields are required: p3 file, the current name of the descriptor, the path to the file<p></p>Rename - renames the descriptor</p> <p>Filled fields are required: p3 file, the current name of the descriptor, the new name of the descriptor, the path to the file.</p>"));

}
void MainWindow::openfile () {

    QString last_path = ui->path_Edit->text();

    //QString last =  QFileDialog::getOpenFileName(this,tr("Open file"),last_path,tr("file ( *.p3 )"));
    last =  QFileDialog::getOpenFileName(this,tr("Open file"),last_path,tr("file ( *.p3 )"));
    QString fwr=last.mid(0,last.lastIndexOf('/'));



    string last1 = last.toStdString();

    strcpy(last1char,last.toLocal8Bit().constData());

    int i;
    for(i = last1.length(); (last1[i] != '\\') && (last1[i] != '/') && i >= 0; --i);
    while (last1[++i] != '\0'){
        file_Name += last1[i];}
    ui->path_Edit->setText(QString (fwr));
    ui->file_Edit->setText(QString::fromStdString(file_Name));

    /*
 // Подставляет описатель, но по факту лучше вбивать в ручную.
  int j;
 `for(j = file_Name.length(); (file_Name[j] != '.') && j >= 0; --j);
  while (file_Name[--j] != '\0'){
        file_Name1 += file_Name[j];}
``int w;
  for(w = file_Name1.length(); (file_Name1[w] == 'D') && w >= 0; --w);
  while (file_Name1[--w] != '\0'){
         file_Name2 += file_Name1[w];}
  ui->descriptor_Edit->setText(QString::fromStdString(file_Name2));
*/


}

void MainWindow::on_aboutM_triggered()
{
    aboutinfo();
}


void MainWindow::on_actionStart_triggered()
{
    openfile();
}

void MainWindow::on_actionExit_triggered()
{
    exit(0);
}

void MainWindow::proverka()
{

    if(ui->file_Edit->text().isEmpty())
    {
        QMessageBox::warning(this,tr("Error"),tr("Mistake. Missing file name."));
        q=false;
        return;
    } else
    {
      q=true;
    }
    if(ui->descriptor_Edit->text().isEmpty())
    {
        QMessageBox::warning(this,tr("Error"),tr("Mistake. Missing current descriptor name."));
        q=false;
        return;
    } else
    {
      q=true;
    }

    if(ui->path_Edit->text().isEmpty())
    {
        QMessageBox::warning(this,tr("Error"),tr("Mistake. Missing file path."));
        q=false;
        return;
    } else
    {
      q=true;
    }

    QString FileName1 = Offset+"/"+File;

    FILE * Firmware=fopen(FileName1.toLocal8Bit(),"rt");
    if(Firmware==NULL){
        QMessageBox::warning(this,tr("Error"),tr("Mistake. There is no file in the directory."));
        q=false;
        return;
    } else
    {
      q=true;
    }

    if(TName =="DSxx")
    {
        QMessageBox::warning(this,tr("Error"),tr("Mistake. Missing current descriptor name."));
        q=false;
        return;
    } else
    {
      q=true;
    }

}

void MainWindow::proverka2()
{
if (q==true){
    if(ui->descriptor_Edit_2->text().isEmpty())
    {
        QMessageBox::warning(this,tr("Error"),tr("Mistake. Missing new descriptor name.The value is taken by default."));
        ui->descriptor_Edit_2->setText(QString (TName));
        return;
    }

    if(TNameOut =="DSXX")
    {
        QMessageBox::warning(this,tr("Error"),tr("Mistake. Missing new descriptor name.The value is taken by default."));
        ui->descriptor_Edit_2->setText(QString (TName));
        return;
    }
}
}

void MainWindow::on_Start_Button_clicked()
{
    File = ui->file_Edit->text();
    TName = ui->descriptor_Edit->text();
    Offset = ui->path_Edit->text();
    ui->descriptor_Edit_2->setText(QString (TName));
    proverka();
    fix();
}

void MainWindow::on_Start_Button_2_clicked()
{
    File = ui->file_Edit->text();
    TName = ui->descriptor_Edit->text();
    TNameOut = ui->descriptor_Edit_2->text();
    Offset = ui->path_Edit->text();
    proverka();
    proverka2();
    fixm();
}


void MainWindow::on_exit_Button_clicked()
{
    exit(0);
}

// Windows  _fseeki64 _ftelli64
void MainWindow::fixm()
{

    if (q==true){

        int offset = Offset.toInt();
        string mc = File.toStdString();
        string tName = TName.toStdString();
        string tName_out = TNameOut.toStdString();

        uint8_t ttName_in[4];
        ttName_in[0] = (uint8_t)tName.c_str()[0];
        ttName_in[1] = (uint8_t)tName.c_str()[1];
        ttName_in[2] = (uint8_t)tName.c_str()[2];
        ttName_in[3] = (uint8_t)tName.c_str()[3];

        uint8_t ttName_read[4];
        uint8_t ttName[4];
        ttName[0] = (uint8_t)tName_out.c_str()[0];
        ttName[1] = (uint8_t)tName_out.c_str()[1];
        ttName[2] = (uint8_t)tName_out.c_str()[2];
        ttName[3] = (uint8_t)tName_out.c_str()[3];

         FILE *fid;
      //  if((fid=fopen(mc.c_str(), "rb+"))==NULL) {}
        QString dirconfig = Offset+"/"+File;
        string last2 = dirconfig.toStdString();
        if((fid=fopen(last2.c_str(), "rb+"))==NULL) {
        }

        _fseeki64( fid , 0 , SEEK_END );
        int64_t  szf = _ftelli64(fid);
        _fseeki64( fid , 0 , SEEK_SET );
        _fseeki64( fid , offset , SEEK_SET );
        int64_t nb = offset;
        int64_t st = 0;
        int16_t c;
        while(1){
            if (nb > szf-2){ break; }
            _fseeki64( fid , nb , SEEK_SET );
            fread(&c, 1, sizeof(uint16_t), fid);       // считываем файл в буфер
            if (c == 9306){
                fseek(fid , 14 , SEEK_CUR );
                fread(ttName_read, 4, sizeof(uint8_t), fid);       // считываем файл в буфер
                if (
                        ttName_read[0] == ttName_in[0] &&
                        ttName_read[1] == ttName_in[1] &&
                        ttName_read[2] == ttName_in[2] &&
                        ttName_read[3] == ttName_in[3] )
                {
                    _fseeki64(fid , -4, SEEK_CUR );
                    fwrite(&ttName[0], sizeof(uint8_t), 1, fid);
                    fwrite(&ttName[1], sizeof(uint8_t), 1, fid);
                    fwrite(&ttName[2], sizeof(uint8_t), 1, fid);
                    fwrite(&ttName[3], sizeof(uint8_t), 1, fid);
                    st = st+1;
                }

            }
            nb = nb + 1;
            if(st == 2){ break; }
        }
        if (ttName_read[0] == ttName_in[0] &&
            ttName_read[1] == ttName_in[1] &&
            ttName_read[2] == ttName_in[2] &&
            ttName_read[3] == ttName_in[3])
       {
            QMessageBox::warning(this,tr("Finish"),tr("Successfully. Descriptor renamed."));
       } else
       {
            QMessageBox::warning(this,tr("Error"),tr("Mistake. The name of the source descriptor is incorrect. The name of the buffer descriptor will not be renamed."));
        }
        fclose(fid);
        //QMessageBox::warning(this,tr("Finish"),tr("Successfully. Descriptor renamed."));
    }
}




