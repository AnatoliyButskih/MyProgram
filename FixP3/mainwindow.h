#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <string>
#include <QTranslator>
#include <QSettings>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void fix();
    void fixm();
    void setArgs();
    void proverka();
    void proverka2();
    void aboutinfo();
    void openfile();

private:
    Ui::MainWindow *ui;
    MainWindow * assignment;
public:
    std::string file_Name;
    std::string file_Name1;
    std::string file_Name2;
    QString mc1;
    QString tName1;
    QString tName1Out;
    QString offset1;
    QString File;
    QString Offset;
    QString TName;
    QString TNameOut;
    char last1char[100];
    QString last;
    bool q = true;
    int ttNameOk;


private slots:
    void on_Start_Button_clicked();
    void on_exit_Button_clicked();
    void on_Start_Button_2_clicked();
    void on_aboutM_triggered();
    void on_actionStart_triggered();
    void on_actionExit_triggered();
};



#endif // MAINWINDOW_H
