#include "mainwindow.h"
#include <QGridLayout>
#include <QDialog>
#include <QFileDialog>
#include <QHeaderView>
#include <QRegExp>
#include "ui_mainwindow.h"
#include <QVBoxLayout>
#include <QMessageBox>
#include <QTime>
#include <iostream>
#include "CommandLineParams.h"
#include "ipaddressmodel.h"
#include <string.h>
#include <QDir>
#include <cstdio>
#include "filesystemmodel.h"
#include "confirmlistmodel.h"

//#include <Windows.h>//Windows

const QString SettingsName::FW_IN_FOLDER=QString("FW_IN_FOLDER");           //папка из которой загружается прошивка
const QString SettingsName::FW_IN_FW_FOLDER=QString("FW_IN_FW_FOLDER");     //папка в которой хранится подгружаемая прошивка
const QString SettingsName::OUT_FW_FOLDER=QString("OUT_FW_FOLDER");         //папка в котрую сохраняются прошивка
const QString SettingsName::EXTRACT_FW_FOLDER=QString("EXTRACT_FW_FOLDER"); //папка в котрую извлекаются прошивка
const QString SettingsName::LAST_PATH=QString("LAST_PATH");
const QString SettingsName::WINDOW_GEOMETRY=QString("WINDOW_GEOMETRY");     //геометрия окна

FOThread::FOThread(MainWindow *window, QObject *parent)
    :QThread(parent)
{
    this->window=window;
    mode=0;
}

void FOThread::startOperation(int mode){
    this->mode=mode;
    start();
}

void FOThread::run()
{

    switch(mode){
    case OPEN_FW_FROM_FOLDER:
        fillDataFromFolder();
        break;
    case OPEN_FW_FROM_FW:
        fillDataFromFW();
        break;
    case CREATE_FW:
        writeNewFW();
        break;
    case EXTRACT_FW:
        extractData();
        break;
    }
    mode=0;
}

void FOThread::fillDataFromFolder(){
    window->fs_model->clear();
    window->conf_model->clear();
    window->fs_model->addDir(window->in_firmware_path);


    emit loadFilesInfo(window->fs_model->getTotalFile(), window->fs_model->getTotalSize());
}
void FOThread::fillDataFromFW()
{
    int ret=window->fwreader->init_read(window->in_firmware_name);
    if(ret!=FWReader::NO_READ_ERROR){
        window->fwreader->clear();
        emit loadError(ret);
        return;
    }
    emit versionFW(window->fwreader->getVersionFW());
    emit pathOnServer(window->fwreader->getPathOnServer());
    emit hostName(window->fwreader->getHostName());
    emit sshIP(window->fwreader->getIP_SSH());
    emit sshPort((int)window->fwreader->getPortSSH());
    emit sshLogin(window->fwreader->getLoginSSH());
    emit sshPassword(window->fwreader->getPasswdSSH());
    emit magicFirmware(window->fwreader->getMagic());
    emit commentFW(window->fwreader->getComment());
    window->fs_model->clear();//очистка списка файлов перед добавлением
    window->conf_model->clear();//очистка листа подтверждения перед добавлением новый файлов
    window->fs_model->addZipStructure(window->fwreader->getZipFileStructure());
    window->fs_model->addConfirmList(window->fwreader->getConfirmList());
    emit loadFilesInfo(window->fs_model->getTotalFile(), window->fs_model->getTotalSize());
    //window->fillStatusBar();

}

void FOThread::writeNewFW()
{
    window->fwwriter->write_firmware(window->fwdata,window->fwreader);
}

void FOThread::extractData()
{
    window->fwreader->extractFW(window->extract_path);
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(QString("FW Creator %1").arg(VERSION));
    setWindowIcon(QIcon(":/icons/tool.png"));
    restoreSettings();//восставнавливаем настройки интерфейса
    fs_model=new FileSystemModel;
    conf_model=new ConfirmListModel;
    ip_addr_model=new IPAddressModel;
    ip_addr_delegate=new IPAddressDelegate(ui->ipAddrList);

    ui->treeView->setModel(fs_model);
    ui->tableView->setModel(conf_model);
    ui->ipAddrList->setModel(ip_addr_model);
    ui->ipAddrList->setItemDelegateForColumn(0,ip_addr_delegate);


    ip_reg_exp=new QRegExp( "\\b(([01]?\\d?\\d|2[0-4]\\d|25[0-5])\\.){3}([01]?\\d?\\d|2[0-4]\\d|25[0-5])\\b");
    path_reg_exp=new QRegExp("(/[a-zA-Z0-9_.]+)+");
    path_valid=new QRegExpValidator(*path_reg_exp,ui->pathEdit);


    ui->treeView->header()->setStretchLastSection(false);
#ifdef HAVE_QT5
    ui->treeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->treeView->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
#else
    ui->treeView->header()->setResizeMode(QHeaderView::ResizeToContents);
    ui->treeView->header()->setResizeMode(0,QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
#endif

    ui->tableView->horizontalHeader()->setStretchLastSection(true);

    total_files=new QLabel(tr("Total files load: -"));
    total_size=new QLabel(tr("Total size load: -"));
    total_path=new QLineEdit(tr("-"));
    total_path->setReadOnly(true);

    ui->sshPortSpin->setRange(0,65535);
    ui->sshPortSpin->setValue(22);


    fwdata=new FWData();
    fwwriter=new FWWriter;
    fothread=new FOThread(this);
    fwreader=new FWReader;

    load_progress=new QMovie(":/icons/load.gif");
    load_label=new QLabel;
    load_label->setMovie(load_progress);
    QVBoxLayout * load_layout=new QVBoxLayout;
    load_layout->addWidget(load_label,1,Qt::AlignCenter);
    ui->treeView->setLayout(load_layout);

    ui->statusbar->addWidget(total_files);
    ui->statusbar->addWidget(total_size);
    ui->statusbar->addWidget(new QLabel(tr("Current path: ")));
    ui->statusbar->addWidget(total_path);


    default_fw_name.append("install_");
    default_fw_name.append(QDate::currentDate().toString("yy_MM_dd"));
    ui->versionEdit->setText(default_fw_name);
    connect(ui->actionOpen,SIGNAL(triggered()),this,SLOT(openFolderFW()));//открыть папку содержащую прошивку
    connect(ui->actionOpenFW,SIGNAL(triggered()),this,SLOT(openFW()));//открыть прошивку
    connect(ui->actionCreateFW,SIGNAL(triggered()),this,SLOT(startFirmwareCreate()));//сгенерировать прошивку
    connect(ui->actionExtractFW,SIGNAL(triggered()),this,SLOT(extractFW()));//извлечь прошивку
    /*Сигналы управления отображения прогресом*/
    connect(fothread,SIGNAL(started()),this,SLOT(startIOThread()));
    connect(fothread,SIGNAL(finished()),this,SLOT(finishIOThread()));

    connect(fothread,SIGNAL(loadError(int)),this,SLOT(errorLoad(int)));

    connect(fothread,SIGNAL(versionFW(QString)),ui->versionEdit,SLOT(setText(QString)));
    connect(fothread,SIGNAL(pathOnServer(QString)),ui->pathEdit,SLOT(setText(QString)));
    connect(fothread,SIGNAL(hostName(QString)),ui->hostnameEdit,SLOT(setText(QString)));
    connect(fothread,SIGNAL(sshIP(QStringList)),ip_addr_model,SLOT(setIPAdress(QStringList)));
    connect(fothread,SIGNAL(sshPort(int)),ui->sshPortSpin,SLOT(setValue(int)));
    connect(fothread,SIGNAL(sshLogin(QString)),ui->loginSSH_Edit,SLOT(setText(QString)));
    connect(fothread,SIGNAL(sshPassword(QString)),ui->passwordSSH_Edit,SLOT(setText(QString)));
    connect(fothread,SIGNAL(magicFirmware(QString)),ui->magic_edit,SLOT(setText(QString)));
    connect(fothread,SIGNAL(commentFW(QString)),ui->commentText,SLOT(setPlainText(QString)));

    connect(fothread,SIGNAL(loadFilesInfo(quint64,quint64)),this,SLOT(fillStatusBar(quint64,quint64)));
    connect(fwwriter,SIGNAL(writeError(int)),this,SLOT(errorWrite(int)));//запись данных
    connect(fwwriter,SIGNAL(writeStatus(int)),this,SLOT(statusWrite(int)));

    connect(fwreader,SIGNAL(readError(int)),this,SLOT(errorRead(int)));
    connect(fwreader,SIGNAL(readStatus(int)),this,SLOT(statusRead(int)));

    connect(fs_model,SIGNAL(selectFile(const QModelIndex &)),conf_model,SLOT(addSelectFile(const QModelIndex &)));
    connect(fs_model, SIGNAL(warningFile(const QString &)), this, SLOT(showWarningFile(const QString &)));
    connect(conf_model,SIGNAL(changeCheckState(const QModelIndex&, const QVariant&)),fs_model,SLOT(setCheckState(const QModelIndex&, const QVariant&)));
    connect(conf_model,SIGNAL(sendRemoveSelectFromHdd(const QModelIndex&)),fs_model,SLOT(removeSelectFromHdd(const QModelIndex&)));
    connect(conf_model,SIGNAL(countConfirmList(const int&)),this, SLOT(setCountConfirmList(const int&)));
    connect(ui->tableView,SIGNAL(removeSelected(const QModelIndexList & )),      conf_model,SLOT(removeSelected(const QModelIndexList & )));
    connect(ui->tableView,SIGNAL(removeSelectedFromHdd(const QModelIndexList &)),conf_model,SLOT(removeSelectedFromHdd(const QModelIndexList & )));

    /*--Удаление из списка IP адресов-*/
    connect(ui->ipAddrList,SIGNAL(removeSelected(const QModelIndexList)),ip_addr_model,SLOT(removeSelected(const QModelIndexList &)));
    connect(ui->ipAddrList,SIGNAL(addIPField()),ip_addr_model,SLOT(addIPField()));
    /*================================*/

    connect(ui->actionAbout,SIGNAL(triggered()),this,SLOT(aboutFWCreator()));
    connect(ui->actionconsole_mode_program,SIGNAL(triggered()),this,SLOT(aboutConsoleMode()));
    connect(ui->actionExit,SIGNAL(triggered()),this,SLOT(close()));
}

void MainWindow::setCountConfirmList(const int &count)
{
    QString text = tr("Confirm List");
    ui->tabWidget->setTabText(1, QString(tr("%1: %2")).arg(text).arg(count));
}

//предупреждение на особые файлы, например calibr.cfg
void MainWindow::showWarningFile(const QString &filename)
{
    qDebug() << __FUNCTION__ << filename;

    QMessageBox msgBox;
    msgBox.setText(tr("Обнаружен особый файл!"));
    msgBox.setInformativeText(QString("%1: [%2]").arg(tr("Удалите файл в окне списка подтверждения")).arg(filename));
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.setBaseSize(QSize(800, 200));
    msgBox.exec();

    ui->tabWidget->setCurrentIndex(1);
    ui->tableView->setFocus();
    ui->pathEdit->setValidator(path_valid);
}

//предупреждение на особые файлы перед созданием прошивки
int MainWindow::showWarningFileOnCreateFW(const QString warning_file)
{
    QMessageBox msgBox;
    msgBox.setText(tr("Обнаружен особый файл!"));
    msgBox.setInformativeText(QString("%1: [%2]")
                              .arg(tr("Проигнорируйте и продолжайте создание прошивки или отмените и удалите файл в окне списка подтверждения"))
                              .arg(warning_file)
                              );
    msgBox.setStandardButtons(QMessageBox::Ignore | QMessageBox::Abort);
    msgBox.setDefaultButton(QMessageBox::Abort);
    msgBox.setBaseSize(QSize(800, 200));
    int res = msgBox.exec();

    if (res == QMessageBox::Abort) {
        ui->tabWidget->setCurrentIndex(1);
        ui->tableView->setFocus();
        return 1;
    }

    return 0;
}
MainWindow::~MainWindow()
{
    delete ui;
    if(fwreader!=NULL){
        delete fwreader;
    }
    if(fwwriter!=NULL){
        delete fwwriter;
    }
    delete conf_model;
    delete fs_model;

}

void MainWindow::openFW(){
    in_firmware_name=QFileDialog::getOpenFileName(this,tr("Open firmware file"),last_path,tr("Update FW ( *.bin ) "));
    last_path = in_firmware_name;

    if(in_firmware_name.isEmpty()){
        return;
    }

    if(fwreader->isInit()){
        fwreader->clear();
        clearInputField();
    }


    fw_in_fw_folder=in_firmware_name;
    fw_in_fw_folder.replace('\\','/');
    fw_in_fw_folder=fw_in_fw_folder.mid(0,fw_in_fw_folder.lastIndexOf('/'));
    settings->setValue(SettingsName::FW_IN_FW_FOLDER,fw_in_fw_folder);
    fothread->startOperation(FOThread::OPEN_FW_FROM_FW);

    ui->magic_edit->setText(QString (tr("loading...")));
}


void MainWindow::openFWUnpack()
{

    cout<<"Unpack firmware mode"<<"\n";


    QString FileName = path_in_clp;
    in_firmware_name = FileName;
    FILE * Firmware=fopen(FileName.toLocal8Bit(),"rt");
    if(Firmware==NULL){
        cout<<"Error opening firmware"<<"\n";
        system("pause");
        exit(0);
    }else {
        cout<<"1/2 Firmware open successfully"<<"\n";
    }

    if(in_firmware_name.isEmpty()){
        return;
    }

    if(fwreader->isInit()){
        fwreader->clear();
        clearInputField();
    }

    fw_in_fw_folder=in_firmware_name;
    fw_in_fw_folder.replace('\\','/');
    fw_in_fw_folder=fw_in_fw_folder.mid(0,fw_in_fw_folder.lastIndexOf('/'));
    settings->setValue(SettingsName::FW_IN_FW_FOLDER,fw_in_fw_folder);
    fothread->startOperation(FOThread::OPEN_FW_FROM_FW);

}

void MainWindow::extractFWUnpack()
{


    QString FileName = path_out_clp;
    extract_path = FileName;
    if(extract_path.isEmpty()){
        return;
    }
    //fsleep();
    extract_fw_folder=extract_path;
    settings->setValue(SettingsName::EXTRACT_FW_FOLDER,extract_fw_folder);
    fsleep();
    fothread->startOperation(FOThread::EXTRACT_FW);
    fsleep();

    QString dirconfig = path_out_clp + "/"+"config";
    QString dircontrol = path_out_clp + "/" + "control";
    QString dirsystem = path_out_clp + "/" + "system";

    QDir dirs(dirconfig);
    if (!dirs.exists()){
        cout<<"Missing configuration file."<<"\n";
        system("pause");
        exit(0);
    }
    QDir dirs1(dircontrol);
    if (!dirs1.exists()){
        cout<<"Missing control directory"<<"\n";
        system("pause");
        exit(0);
    }
    QDir dirs2(dirsystem);
    if (!dirs2.exists()){
        cout<<"Missing system catalog"<<"\n";
        system("pause");
        exit(0);
    }
    cout<<"2/2 Firmware successfully unpacked!"<<"\n";
    exit(0);
}


void MainWindow::openFolderFW()
{
    in_firmware_path=QFileDialog::getExistingDirectory(this, tr("Select firmware directory"),last_path,QFileDialog::ShowDirsOnly);
    last_path = in_firmware_path;

    if(in_firmware_path.isEmpty())
    {
        return;
    }
    if(fwreader->isInit()){
        fwreader->clear();
        clearInputField();
    }

    fw_in_folder=in_firmware_path;
    settings->setValue(SettingsName::FW_IN_FOLDER,fw_in_folder);
    fothread->startOperation(FOThread::OPEN_FW_FROM_FOLDER);

}


void MainWindow::startFirmwareCreate()
{

    if (mode_clp == "3"){
       // QString mag = ui->MagComboBox->currentText();
        QString mag = ui->magic_edit->text();
        if (mag == "0x245B245A") {
            m3 = 0;
        } else {
            m3 = 1;
            ui->magic_edit->setText(QString ("0x245B245B"));
        }
    }

    QString warning_file = fs_model->checkWarnings();
    if (!warning_file.isEmpty()) {
        if (showWarningFileOnCreateFW(warning_file)!=0)
            return;
    }

    if(ui->versionEdit->text().isEmpty())//проверяем что поле версии ПО содержит информацию
    {
        QMessageBox::warning(this,tr("FW version"),tr("Firmware version isn't specify"));
        return;
    }

    if(ui->pathEdit->text().isEmpty())//проверяем что поле пути копирования на сервер содержит информацию
    {
        QMessageBox::warning(this,tr("FW path"),tr("Firmware path on server isn't specify"));
        return;
    }

    if(!path_reg_exp->exactMatch(ui->pathEdit->text())){
        QMessageBox::warning(this,tr("FW path"),tr("Firmware path on server isn't valid"));
        return;
    }

    if(ui->hostnameEdit->text().isEmpty()){
        QMessageBox::warning(this,tr("Host name"),tr("Host name isn't specify"));
        return;
    }



    if(!ip_addr_model->isValid()){//проверяем наличие IP адреса в поле ввода
        QMessageBox::warning(this,tr("SSH IP"),tr("SSH IP address in list isn't valid"));
        return;
    }



    if(ui->loginSSH_Edit->text().isEmpty())//проверяем что поле логина SSH не пустое
    {
        QMessageBox::warning(this,tr("FW SSH login"),tr("Login SSH isn't specify"));
        return;
    }

    if(ui->passwordSSH_Edit->text().isEmpty()){//проверяем что поле пароля SSH не пустое
        QMessageBox::warning(this,tr("FW SSH password"),tr("Password SSH isn't specify"));
        return;
    }
    if(fs_model->getTotalFile()==0){
        QMessageBox::warning(this,tr("FW data"),tr("Firmware data isn't specify"));
        return;
    }



    out_firmware_path = QFileDialog::getSaveFileName(this,tr("Select out bin"),last_path+"/"+ui->versionEdit->text()+".bin",tr("Bin file ( *.bin )"));

    if(out_firmware_path.isEmpty()){
        return;
    }
    last_path = out_firmware_path;

    if(!in_firmware_name.compare(out_firmware_path)){
        QMessageBox::warning(this,tr("FW source"),tr("Firmware input and output files are equal"));
        return;
    }

    out_fw_folder=out_firmware_path;
    out_fw_folder.replace('\\','/');
    out_fw_folder=out_fw_folder.mid(0,out_fw_folder.lastIndexOf('/'));
    settings->setValue(SettingsName::OUT_FW_FOLDER,out_fw_folder);

    fwdata->reset();
    fwdata->setVersion(ui->versionEdit->text());
    fwdata->setPathOnServer(ui->pathEdit->text());
    fwdata->setHostName(ui->hostnameEdit->text());
    fwdata->setConfirmList(conf_model->getConfirmList());
    fwdata->setLoadPath(in_firmware_path);

    fwdata->setIP_SSH(ip_addr_model->getIPList());
    fwdata->setPortSSH(ui->sshPortSpin->value());
    fwdata->setLoginSSH(ui->loginSSH_Edit->text());
    fwdata->setPasswdSSH(ui->passwordSSH_Edit->text());

    fwdata->setComment(ui->commentText->toPlainText());
    fwdata->setOutFWName(out_firmware_path);
    fothread->startOperation(FOThread::CREATE_FW);
}

void MainWindow::extractFW(){
    extract_path=QFileDialog::getExistingDirectory(this, tr("Select firmware directory"),last_path,QFileDialog::ShowDirsOnly);
    last_path = extract_path;

    if(extract_path.isEmpty()){
        return;
    }
    extract_fw_folder=extract_path;
    settings->setValue(SettingsName::EXTRACT_FW_FOLDER,extract_fw_folder);
    fothread->startOperation(FOThread::EXTRACT_FW);

}


void MainWindow::restoreSettings()
{
    settings=new QSettings(QSettings::UserScope,"FWCreator");
    fw_in_folder=settings->value(SettingsName::FW_IN_FOLDER,".").toString();
    fw_in_fw_folder=settings->value(SettingsName::FW_IN_FW_FOLDER,".").toString();
    out_fw_folder=settings->value(SettingsName::OUT_FW_FOLDER,".").toString();
    extract_fw_folder=settings->value(SettingsName::EXTRACT_FW_FOLDER,".").toString();
    last_path=settings->value(SettingsName::LAST_PATH,".").toString();
    restoreGeometry(settings->value(SettingsName::WINDOW_GEOMETRY).toByteArray());
}

void MainWindow::closeEvent(QCloseEvent *event){
    settings->setValue(SettingsName::LAST_PATH, last_path);
    settings->setValue(SettingsName::WINDOW_GEOMETRY, saveGeometry());
    QMainWindow::closeEvent(event);
}
void MainWindow::startIOThread(){
    load_label->show();
    load_progress->start();
}

void MainWindow::finishIOThread(){
    load_progress->stop();
    load_label->hide();
}

void MainWindow::errorLoad(int code){
    Q_UNUSED(code);
    QMessageBox::critical(this,tr("FW read"),tr("Firmware file is corrupt"));
}

void MainWindow::errorRead(int code){
    switch(code){
    case FWReader::ERROR_EXTRACT_DATA:
        QMessageBox::warning(this,tr("Extract data"),tr("Error extract data from firmware"));
        break;
    default:
    {
        QMessageBox::critical(this,tr("Read params"),tr("Error read params from firmware"));
        printf("Error code %u\n",code);
        fflush(stdout);
    }
        break;
    }
}

void MainWindow::statusRead(int code){
    switch (code){
    case FWReader::SUCCESS_EXTRACT_DATA:
        QMessageBox::information(this,tr("Extract data"),tr("Data extracted successfull"));
        break;
    }
}

void MainWindow::errorWrite(int code)
{
    switch(code){
    case FWWriter::ERROR_CREATE_TMP_FILE:
        QMessageBox::critical(this,tr("FW write"),tr("Can not create temp file"));
        break;
    case FWWriter::ERROR_WRITE_FILE_HEADER:
        QMessageBox::critical(this,tr("FW write"),tr("Can not write file header"));
        break;
    case FWWriter::ERROR_CREATE_CHIPER_FILE:
        QMessageBox::critical(this,tr("FW write"),tr("Can not create firmware file"));
        break;
    case FWWriter::ERROR_WRITE_CHIPER_FILE:
        QMessageBox::critical(this,tr("FW write"),tr("Can not write firmware file"));
        break;
    default:
        QMessageBox::critical(this,tr("FW write"),tr("Can not write file sections"));
    }
}

void MainWindow::statusWrite(int code)
{
    switch(code)
    {
    case FWWriter::SUCCESS_WRITE_FIRMWARE:

        break;
    }
}


void MainWindow:: aboutFWCreator(){
    BuildDate.append(QDate::currentDate().toString("yy_MM_dd"));
    QMessageBox::about(this,tr("About FW Creator"),tr("<h3>FW Creator %1</h3>"
                                                      "<p> </p>"
                                                      "<p>Copyright &copy; 2008-2020 Infotrans</p>"
                                                      "<p>Developer: Viktor Butskikh NIO-5</p>"
                                                      "<p>Developer: Simakov Dmitrii DPR</p>"
                                                      "<p>Developer: Anatoliy Butskikh DPR 20-05-13</p>"
                                                      "<p>Build Qt version %2</p>"
                                                      "<p>Current Qt version %3</p>").arg(VERSION).arg(QT_VERSION_STR).arg(qVersion()).arg(BuildDate));
}
void MainWindow:: aboutConsoleMode(){
    QMessageBox::about(this,tr("About FW Creator"),tr("<h3>FW Creator Commands start </h3>"
                                                      "<p> key = value </p>"
                                                      "<p> -help=1 information</p>"
                                                      //"<p> '-t' technological delay (10000 milliseconds)</p>" //Windows
                                                      "<p> '-t' technological delay (10 sec)</p>" //Linux
                                                      "<p> '-pc' copy path on controller (/home/firmware)</p>"
                                                      "<p> '-mode' program mode key </p>"
                                                      "<h4> -mode=1 Creating firmware in command mode</h4>"
                                                      //"<p> example: .\\FWCreator -mode=1 -l=root -p=root -c=K000 " //Windows
                                                      //"<p> -o=C:\\Users\\bin -v=ps100 -i=C:\\Users\\firmware </p>" //Windows
                                                      "<p> example: ./FWCreator -mode=1 -l=root -p=root -c=K000 " //Linux
                                                      "<p> -o=/home/bin/ -v=ps100 -i=/home/firmware/ </p>" //Linux
                                                      "<p> '-m' magic code </p>"
                                                      "<p> -m=old  magic= 0x245A245A(old) </p>"
                                                      "<p> -m=new (default) (any value except old) 0x245A245B (new)</p>"
                                                      "<p> '-v'  firmware version (name railway carriage) (PS) </p>"
                                                      "<p> '-l' login (root)</p>"
                                                      "<p> '-p'  password (root) </p>"
                                                      "<p> '-ip' controller ip address  (192.168.0.120) </p>"
                                                      //"<p> '-i' firmware source path (C:\\Users\\firmware)</p>"//Windows
                                                      //"<p> '-o' path to save firmware (C:\\Users\\bin)</p>"//Windows
                                                      "<p> '-i' firmware source path (/home/firmware/)</p>"//Linux
                                                      "<p> '-o' path to save firmware (/home/bin/)</p>"//Linux
                                                      "<p> '-c' controller name  (K000) </p>"
                                                      "<p> '-calibr' exception file calibr.cfg from firmware  (0), 1 - remove from firmware, 2 - remove from disk</p>"
                                                      "<p> '-geometry' exception file geometry.cfg from firmware  (0), 1 - remove from firmware, 2 - remove from disk</p>"
                                                      "<p> '-s' suffix</p>"
                                                      "<h4> -mode=2 firmware unpacking mode</h4>"
                                                      //"<p> example: .\\FWCreator -mode=2 -i=C:\\Users\\bin\\install_20_04_06_ps100.bin -o=C:\\Users\\firmware</p>" //Windows
                                                      //"<p> '-i' path to firmware and name of firmware (C:\\Users\\bin\\*.bin)</p>" //Windows
                                                      //"<p> '-o' path to unpack the firmware (C:\\Users\\firmware0)</p>"//Windows
                                                      "<p> example: ./FWCreator -mode=2 -i=/home/bin/install_20_04_06_ps100.bin -o=/home/firmware/</p>" //Linux
                                                      "<p> '-i' path to firmware and name of firmware (/home/bin/*.bin)</p>" //Linux
                                                      "<p> '-o' path to unpack the firmware (/home/firmware0/)</p>"//Linux
                                                      "<h4> -mode=3 graphic mode </h4>"
                                                      //"<p> example: .\\FWCreator -mode=3(undesirable) -l=root -p=root -c=K000 -v=ps100</p>"//Windows
                                                      "<p> example: ./FWCreator -mode=3(undesirable) -l=root -p=root -c=K000 -v=ps100</p>"//Linux
                                                      "<p> '-i' and '-o' not used</p>"
                                                      ));
}

void MainWindow::fillStatusBar(quint64 file_count, quint64 file_size)
{
    total_files->setText(tr("Total files load: ").append(QString::number(file_count)));
    total_path->setText(in_firmware_path);

    uint64_t tera=1099511627776ULL;//1 терабайт данных
    uint64_t giga=1024*1024*1024;
    uint64_t mega=1024*1024;
    uint64_t kilo=1024;


    QString  size_str;

    if(file_size > tera){//определяем какой размер файла
        size_str = QString::number((((double)file_size) / tera),'f',2).append(tr(" Tb"));
    }else if(file_size > giga){
        size_str = QString::number((((double)file_size) / giga),'f',2).append(tr(" Gb"));
    }else if(file_size > mega){
        size_str = QString::number((((double)file_size) / mega),'f',2).append(tr(" Mb"));
    }else if(file_size > kilo){
        size_str = QString::number((((double)file_size) / kilo),'f',2).append(tr(" Kb"));
    }else{
        size_str = QString::number(file_size);
    }
    total_size->setText(tr("Total size load: ").append(size_str));
}

void MainWindow::clearInputField(){
    ui->versionEdit->clear();
    ui->pathEdit->clear();
    ui->hostnameEdit->clear();
    ui->loginSSH_Edit->clear();
    ui->passwordSSH_Edit->clear();
    ui->commentText->clear();
}

void MainWindow::setArgs()
{

    cout<<"Creating firmware in command mode"<<"\n";

    if (login_clp == "root"){
        cout<<"1/11 login initialized. default "<<"\n";
    } else {
        cout<<"1/11 login initialized."<<"\n";
    }

    if (passwd_clp == "root"){
        cout<<"2/11 passwd initialized. default "<<"\n";
    } else {
        cout<<"2/11 passwd initialized."<<"\n";
    }

    if (version_clp1 == "PS"){
        cout<<"3/11 version initialized. default"<<"\n";
    } else {
        cout<<"3/11 version initialized."<<"\n";
    }

    // проверка исходных данных прошивки


    QString dirconfig = path_in_clp + "/"+"config"; //Linux
    QString dircontrol = path_in_clp + "/" + "control"; //Linux
    QString dirsystem = path_in_clp + "/" + "system"; //Linux

    QDir dirs(dirconfig);
    if (!dirs.exists()){
        cout<<"Source data error. Missing config directory"<<"\n";
        system("pause");
        exit(0);
    }
    QDir dirs1(dircontrol);
    if (!dirs1.exists()){
        cout<<"Source data error. Missing directory control"<<"\n";
        system("pause");
        exit(0);
    }
    QDir dirs2(dirsystem);
    if (!dirs2.exists()){
        cout<<"Source data error. Missing directory system"<<"\n";
        system("pause");
        exit(0);
    }


    QString  path_in = path_in_clp;
    if (path_in == "C:\\Users\firmware"){
        cout<<"4/11 firmware source path initialized. default"<<"\n";
    } else {
        cout<<"4/11 firmware source path initialized"<<"\n";
    }

    if (path_out_clp == "C:\\Users\bin"){
        cout<<"5/11 path to save firmware  initialized. default"<<"\n";
    } else {
        cout<<"5/11 path to save firmware  initialized"<<"\n";
    }

    if (controller_clp == "K000"){
        cout<<"6/11 controller initialized. default"<<"\n";
    } else {
        cout<<"6/11 controller initialized"<<"\n";
    }


    if (magic_clp == "new"){
        cout<<"7/11 magic_code initialized. default"<<"\n";
    } else {
        cout<<"7/11 magic_code initialized"<<"\n";
    }

    if (magic_clp == "old") {
        m3 = 0;
    } else {
        m3 = 1;
    }


    if (path_on_controller_clp == "/home/firmware/"){
        cout<<"8/11 The path to save the firmware to the controller default"<<"\n";
    } else {cout<<"8/11 The path to save the firmware to the controller"<<"\n";}

    if (portSSH_clp == "22"){
        cout<<"9/11 port ssh initialized. default "<<"\n";
    } else {cout<<"9/11 port ssh initialized"<<"\n";}

    int a;
    if (portSSH_clp != "22"){
        a = portSSH_clp.toInt();
    } else {
        a=22;
    }

    ui->sshPortSpin->setValue(int (a));
    ui->pathEdit->setText(QString (path_on_controller_clp));
    ui->passwordSSH_Edit->setText(QString (passwd_clp));
    ui->loginSSH_Edit->setText(QString (login_clp));
    ui->versionEdit->setText(QString (version_clp));
    ui->hostnameEdit->setText(QString (controller_clp));


    QString g1 = path_in_clp + "config/common/geometry.cfg";

    char gf1[100];

    strcpy(gf1,g1.toLocal8Bit().constData());
    if (geometry_clp == "2"){
        if ( remove(gf1) != 0 ){
            cout << "Error. geometry.cfg not found"<<"\n";
        }else {
            cout << "geometry.cfg successfully deleted "<<"\n";
        }

    }

    QString c1 = path_in_clp + "config/common/calibr.cfg"; //Linux
    char cf1[100];
    strcpy(cf1,c1.toLocal8Bit().constData());

    if (calibr_clp == "2"){
        if ( remove(cf1) != 0 ){
            cout << "Error. calibr.cfg not found"<<"\n";
        }else {
            cout << "calibr.cfg successfully deleted "<<"\n";
        }
    }
}


void MainWindow::setArgsGraficMode()
{

    cout<<"Creating firmware in graphical mode"<<"\n";

    if (login_clp == "root"){
        cout<<"login initialized. default "<<"\n";
    } else {
        cout<<"login initialized."<<"\n";
    }

    if (passwd_clp == "root"){
        cout<<"passwd initialized. default "<<"\n";
    } else {
        cout<<"passwd initialized."<<"\n";
    }

    if (version_clp1 == "PS"){
        cout<<" version initialized. default"<<"\n";
    } else {
        cout<<"version initialized."<<"\n";
    }

    cout<<"firmware source path no initialized. graphic mode"<<"\n";
    cout<<"path to save firmware  no initialized. graphic mode"<<"\n";

    if (controller_clp == "K000"){
        cout<<"controller initialized. default"<<"\n";
    } else {
        cout<<"controller initialized"<<"\n";
    }


    if (path_on_controller_clp == "/home/firmware/"){
        cout<<"The path to save the firmware to the controller default"<<"\n";
    } else {
        cout<<"The path to save the firmware to the controller"<<"\n";
    }

    if (portSSH_clp == "22"){
        cout<<"port ssh initialized. default "<<"\n";
    } else {
        cout<<"port ssh initialized"<<"\n";
    }

    int  a;
    if (portSSH_clp != "22"){
        a = portSSH_clp.toInt(/*0,10*/);
    } else {
        a=22;
    }

    ui->sshPortSpin->setValue(int (a));
    ui->pathEdit->setText(QString (path_on_controller_clp));
    ui->passwordSSH_Edit->setText(QString (passwd_clp));
    ui->loginSSH_Edit->setText(QString (login_clp));
    ui->versionEdit->setText(QString (version_clp));
    ui->hostnameEdit->setText(QString (controller_clp));
    ui->magic_edit->setText(QString (tr("undefined")));
}

void MainWindow::startFirmwareCreateComandMode()
{
    QString FileName;

    if (suffix_clp1 == ""){
        FileName = path_out_clp + version_clp +".bin";
    } else {
        FileName = path_out_clp + version_clp + suffix_clp + ".bin";
    }

    out_firmware_path = FileName;
    //fsleep();
    cout<<"10/11 firmware is being created "<<"\n";


    if(out_firmware_path.isEmpty()){
        return;
    }

    last_path = out_firmware_path;
    //fsleep();
    if(!in_firmware_name.compare(out_firmware_path)){
        QMessageBox::warning(this,tr("FW source"),tr("Firmware input and output files are equal"));
        return;
    }
    //fsleep();
    out_fw_folder=out_firmware_path;
    out_fw_folder.replace('\\','/');
    out_fw_folder=out_fw_folder.mid(0,out_fw_folder.lastIndexOf('/'));
    settings->setValue(SettingsName::OUT_FW_FOLDER,out_fw_folder);

    fwdata->reset();
    fwdata->setVersion(ui->versionEdit->text());
    fwdata->setPathOnServer(ui->pathEdit->text());
    fwdata->setHostName(ui->hostnameEdit->text());
    fwdata->setConfirmList(conf_model->getConfirmList());
    fwdata->setLoadPath(in_firmware_path);

    fwdata->setIP_SSH(ip_addr_model->getIPList());
    fwdata->setPortSSH(ui->sshPortSpin->value());
    fwdata->setLoginSSH(ui->loginSSH_Edit->text());
    fwdata->setPasswdSSH(ui->passwordSSH_Edit->text());

    fwdata->setComment(ui->commentText->toPlainText());
    fwdata->setOutFWName(out_firmware_path);

    if  (calibr_clp=="1"){
        fwdata->addConfirmFile("config/common/calibr.cfg");
    }

    if (geometry_clp=="1"){
        fwdata->addConfirmFile("config/common/geometry.cfg");
    }

    fothread->startOperation(FOThread::CREATE_FW);

    fsleep();

    QString FileName1;
    if (suffix_clp1 == ""){
        FileName1 = path_out_clp + version_clp + ".bin";
    } else {
        FileName1 = path_out_clp + version_clp + suffix_clp + ".bin";
    }

    FILE * Firmware1=fopen(FileName1.toLocal8Bit(),"rt");
    if(Firmware1==NULL){
        cout<<"Error. I do not create a firmware file."<<"\n";
        system("pause");
    }else {
        cout<<"11/11 Firmware assembled "<<"\n";
        exit(0);
    }
    exit(0);
}

void MainWindow::openFolderFWComandMode()
{

    in_firmware_path = path_in_clp;

    if(in_firmware_path.isEmpty())
    {
        return;
    }
    if(fwreader->isInit()){
        fwreader->clear();
        clearInputField();
    }

    fw_in_folder=in_firmware_path;
    settings->setValue(SettingsName::FW_IN_FOLDER,fw_in_folder);
    fothread->startOperation(FOThread::OPEN_FW_FROM_FOLDER);
}

void MainWindow::setArgumentsCLP(){

    mode_clp = CommandLineParser::instance().getValDef("-mode", "3");
    help_clp = CommandLineParser::instance().getValDef("-help", "2");
    geometry_clp = CommandLineParser::instance().getValDef("-geometry", "0");
    calibr_clp = CommandLineParser::instance().getValDef("-calibr", "0");
    login_clp = CommandLineParser::instance().getValDef("-l", "root");
    passwd_clp = CommandLineParser::instance().getValDef("-p", "root");
    version_clp1 = CommandLineParser::instance().getValDef("-v", "PS");
    version_clp = default_fw_name + "_" + version_clp1;
    magic_clp = CommandLineParser::instance().getValDef("-m", "new");
    path_on_controller_clp = CommandLineParser::instance().getValDef("-pc", "/home/firmware");
    portSH_clp = CommandLineParser::instance().getValDef("-port", "22");
    suffix_clp1 = CommandLineParser::instance().getValDef("-s", "");
    suffix_clp = "_" + suffix_clp1;
    controller_clp =CommandLineParser::instance().getValDef("-c", "K000");
    portSSH_clp =CommandLineParser::instance().getValDef("-port", "22");

    time_clp = CommandLineParser::instance().getValDef("-t", "15");// Linux
    path_in_clp = CommandLineParser::instance().getValDef("-i", "C:\\Users\\firmware");
    path_out_clp = CommandLineParser::instance().getValDef("-o", "C:\\Users\\bin");

    //    time_clp = CommandLineParser::instance().getValDef("-t", "15000");// Windows
    //    path_in_clp = CommandLineParser::instance().getValDef("-i", "/home/firmware/");
    //    path_out_clp = CommandLineParser::instance().getValDef("-o", "/home/bin/");
}

void MainWindow::fsleep(){
    int mytime = time_clp.toInt();
    sleep(mytime);//Linux
    //Sleep(mytime);//Windows
}

void MainWindow::on_MagComboBox_activated(const QString &arg1)
{

    QString mag = ui->MagComboBox->currentText();
    ui->magic_edit->setText(QString (mag));
}
