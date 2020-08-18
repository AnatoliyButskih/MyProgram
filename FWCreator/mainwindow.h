#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QApplication>
#include "QMainWindow"
#include <QMovie>
#include <QLabel>
#include <QThread>
#include <QRegExpValidator>
#include <QFileSystemModel>
#include <QDebug>
#include <QTreeView>
#include <QLabel>
#include <QLineEdit>
#include <QGroupBox>
#include <QCheckBox>
#include <QSettings>
#include "filesystemmodel.h"
#include "confirmlistmodel.h"
#include "ipaddressmodel.h"
#include "ipaddressdelegate.h"
#include "fwdata.h"
#include "fwwriter.h"
#include "fwreader.h"

#define VERSION "1.3.2 [2020]"

namespace Ui {
class MainWindow;
}
class MainWindow;

class SettingsName{
public:
    static const QString FW_IN_FOLDER;      //РїР°РїРєР° РёР· РєРѕС‚РѕСЂРѕР№ Р·Р°РіСЂСѓР¶Р°РµС‚СЃСЏ РїСЂРѕС€РёРІРєР°
    static const QString FW_IN_FW_FOLDER;    //РїР°РїРєР° РІРєРѕС‚СЂРѕР№ С…СЂР°РЅРёС‚СЃСЏ РїРѕРґРіСЂСѓР¶Р°РµРјР°СЏ РїСЂРѕС€РёРІРєР°
    static const QString OUT_FW_FOLDER;     //РїР°РїРєР° РІ РєРѕС‚СЂСѓСЋ СЃРѕС…СЂР°РЅСЏСЋС‚СЃСЏ РїСЂС€РёРІРєРё
    static const QString EXTRACT_FW_FOLDER; //РїР°РїРєР° РІ РєРѕС‚СЂСѓСЋ РёР·РІР»РµРєР°СЋС‚СЃСЏ РїСЂРѕС€РёРІРєРё
    static const QString LAST_PATH; //РїР°РїРєР° РІ РєРѕС‚СЂСѓСЋ РёР·РІР»РµРєР°СЋС‚СЃСЏ РїСЂРѕС€РёРІРєРё
    static const QString WINDOW_GEOMETRY;   //РіРµРѕРјРµС‚СЂРёСЏ РѕРєРЅР°
};

class FOThread : public QThread{//РєР»Р°СЃСЃ СЃРѕРґРµСЂР¶Р°С‰РёР№ РїРѕС‚РѕРє С‡С‚РµРЅРёСЏ/Р·Р°РїРёСЃРё РІ С„Р°Р№Р»С‹
    Q_OBJECT
public:
    FOThread(MainWindow  * window, QObject *parent=0);
    void startOperation(int mode);
    void run();
    static const int OPEN_FW_FROM_FOLDER   = 0x11;//РѕС‚РєСЂС‹С‚СЊ РїСЂРѕС€РёРІРєСѓ РёР· РїР°РїРєРё
    static const int OPEN_FW_FROM_FW       = 0x12;//РѕС‚РєСЂС‹С‚СЊ РїСЂРѕС€РёРІРєСѓ РёР· РґСЂСѓРіРѕР№ РїСЂРѕС€РёРІРєРё
    static const int CREATE_FW             = 0x13;//СЃРѕР·РґР°С‚СЊ РїСЂРѕС€РёРІРєСѓ РёР· РґСЂСѓРіРѕР№ РїСЂРѕС€РёРІРєРё
    static const int EXTRACT_FW            = 0x14;//РёР·РІР»РµС‡РµРЅРёРµ РїСЂРѕС€РёРІРєРё
signals:
    void loadError(int code);
    void versionFW(QString version);
    void pathOnServer(QString path_on_server);
    void hostName(QString host_name);
    void sshIP(QStringList ssh_ip);
    void sshPort(int ssh_port);
    void magicCode(int magic_code1);
    void sshLogin(QString login);
    void sshPassword(QString password);
    void magicFirmware(QString magicfirmware);
    void commentFW(QString comment);
    void loadFilesInfo(quint64 file_count, quint64 file_size);

private:
    void fillDataFromFolder();  //Р·Р°РїРѕР»РЅРёС‚СЊ РґР°РЅРЅС‹РјРё РёР· РїР°РїРєРё
    void fillDataFromFW();      //Р·Р°РїРѕР»РЅРёС‚СЊ РґР°РЅРЅС‹РјРё РёР· РґСЂСѓРіРѕР№ РїСЂРѕС€РёРІРєРё
    void writeNewFW();          //Р·Р°РїРёСЃСЊ РЅРѕРІРѕР№ РїСЂРѕС€РёРІРєРё
    void extractData();         //РёР·РІР»РµС‡СЊ РґР°РЅРЅС‹Р№ РёР· РїСЂРѕС€РёРІРєРё
    MainWindow  * window;
    int mode;
};

class MainWindow : public QMainWindow
{
    friend class FOThread;
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void setCountConfirmList(const int &count);
    void showWarningFile(const QString &filename);
    int showWarningFileOnCreateFW(const QString warning_file);

    void setArgs();
    void setArgsGraficMode();
    void setArgumentsCLP();
    void fsleep();

    void startFirmwareCreateComandMode();
    void openFolderFWComandMode();
    void openFWUnpack();//РѕС‚РєСЂС‹С‚СЊ РїСЂРѕС€РёРІРєСѓ
    void extractFWUnpack();//РёР·РІР»РµС‡СЊ РґР°РЅРЅС‹Рµ РїСЂРѕС€РёРІРєРё

private slots:
    void openFolderFW();//РѕС‚РєСЂС‹С‚СЊ РїР°РїРєСѓ СЃРѕРґРµСЂР¶Р°С‰СѓСЋ РїСЂРѕС€РёРІРєСѓ
    void openFW();//РѕС‚РєСЂС‹С‚СЊ РїСЂРѕС€РёРІРєСѓ

    void startFirmwareCreate();//СЃРіРµРЅРµСЂРёСЂРѕРІР°С‚СЊ РїСЂРѕС€РёРІРєСѓ
    void extractFW();//РёР·РІР»РµС‡СЊ РґР°РЅРЅС‹Рµ РїСЂРѕС€РёРІРєРё

    void startIOThread();//завершение потока файловых манипуляций
    void finishIOThread();//завершение потока файловых манипуляций
    void errorLoad(int code);//ошибки загрузки прошивки
    void errorRead(int code);//ошибки чтения прошивки
    void statusRead(int code);//статус чтения данных
    void errorWrite(int code);//ошибки записи прошивки
    void statusWrite(int code);//статус записи данных
    void aboutFWCreator();
    void aboutConsoleMode();
    void fillStatusBar(quint64 file_count, quint64 file_size);//Р·Р°РїРѕР»РЅРёС‚СЊ Status Bar РёРЅС„РѕСЂРјР°С†РёРµР№



    void on_MagComboBox_activated(const QString &arg1);

private:

    void clearInputField();//РѕС‡РёСЃС‚РєР° РїРѕР»РµР№ РІРІРѕРґР°

    void restoreSettings();
    void closeEvent(QCloseEvent *event);


    Ui::MainWindow *ui;
    FileSystemModel * fs_model;
    ConfirmListModel * conf_model;
    IPAddressModel * ip_addr_model;
    IPAddressDelegate * ip_addr_delegate;
    FWData * fwdata;

    QLabel * total_files;
    QLabel * total_size;
    QLineEdit * total_path;
    FWWriter * fwwriter;
    FWReader * fwreader;
    QSettings * settings;
    QString last_path;      //последний использованный путь
    QString in_firmware_name;
    QString in_firmware_path;//РїСѓС‚СЊ РґРѕ С„Р°Р№Р»РѕРІ РїСЂРѕС€РёРІРєРё
    QString out_firmware_path;//РїСѓС‚СЊ СЂРµР·СѓР»СЊС‚РёСЂСѓСЋС‰РµРіРѕ С„Р°Р№Р»Р° РїСЂРѕС€РёРІРєРё
    QString extract_path;


    QString fw_in_folder;
    QString fw_in_fw_folder;
    QString out_fw_folder;
    QString extract_fw_folder;
    QString default_fw_name;
    QString BuildDate;

    FOThread * fothread;//РїРѕС‚РѕРє Р·Р°РїРёСЃРё РІ С„Р°Р№Р»
    QMovie * load_progress;
    QLabel * load_label;
    QRegExp * ip_reg_exp;
    QRegExp * path_reg_exp;
    QRegExpValidator * ip_valid; //РІР°Р»РёРґР°С‚РѕСЂ IP Р°РґСЂРµСЃР°
    QRegExpValidator * path_valid; //РІР°Р»РёРґР°С‚РѕСЂ РїСѓС‚Рё РєРѕРїРёСЂРѕРІР°РЅРёСЏ Р°РґСЂРµСЃР° РЅР° СЃРµСЂРІРµСЂРµ

public:
    QString mode_clp;
    QString help_clp;
    QString path_in_clp;
    QString path_out_clp;
    QString geometry_clp;
    QString calibr_clp;
    QString login_clp;
    QString passwd_clp;
    QString version_clp;
    QString version_clp1;
    QString magic_clp;
    QString path_on_controller_clp;
    QString portSH_clp;
    QString time_clp;
    QString suffix_clp1;
    QString suffix_clp;
    QString controller_clp;
    QString portSSH_clp;
};

#endif // MAINWINDOW_H
