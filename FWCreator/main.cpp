//#include <QtGui/QApplication>
#include "mainwindow.h"
#include <QTranslator>
#include <QCoreApplication>
#include <iostream>
#include <QMap>
#include <QString>
#include <QDebug>
#include "CommandLineParams.h"
#include "fwdata.h"
#include "info.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CommandLineParser::instance().init(a.arguments());
    QTranslator trans;
    trans.load(":/translator/fwcreator_ru");
    a.installTranslator(&trans);
    MainWindow w;
    info t;
    w.setArgumentsCLP();
    t.inform();
    if (w.mode_clp == "1") {
        w.setArgs();
        w.openFolderFWComandMode();
        w.startFirmwareCreateComandMode();
    } if (w.mode_clp == "2") {
        w.openFWUnpack();
        w.extractFWUnpack();
    }else {
        w.setArgsGraficMode();
        w.show();
    }
    return a.exec();
}
