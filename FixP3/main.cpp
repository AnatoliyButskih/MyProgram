#include "mainwindow.h"
#include <QTranslator>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTranslator trans;
    trans.load(":/fixp3_ru");
    a.installTranslator(&trans);

    MainWindow w;
    w.setWindowTitle("FixP3");
    w.setWindowIcon(QIcon("fixp3.png"));
    w.setArgs();
    w.show();
    return a.exec();
}

