/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QStatusBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *aboutM;
    QAction *actionStart;
    QAction *actionExit;
    QWidget *centralwidget;
    QLineEdit *file_Edit;
    QLineEdit *descriptor_Edit;
    QLineEdit *path_Edit;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLineEdit *descriptor_Edit_2;
    QLabel *label_4;
    QPushButton *Start_Button;
    QPushButton *Start_Button_2;
    QPushButton *exit_Button;
    QStatusBar *statusbar;
    QMenuBar *menubar;
    QMenu *menuAbout;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->setWindowModality(Qt::NonModal);
        MainWindow->resize(434, 278);
        MainWindow->setMinimumSize(QSize(434, 278));
        MainWindow->setMaximumSize(QSize(434, 278));
        MainWindow->setMouseTracking(true);
        MainWindow->setProperty("tabletTracking", QVariant(true));
        aboutM = new QAction(MainWindow);
        aboutM->setObjectName(QString::fromUtf8("aboutM"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/icons/icons/about.png"), QSize(), QIcon::Normal, QIcon::Off);
        aboutM->setIcon(icon);
        actionStart = new QAction(MainWindow);
        actionStart->setObjectName(QString::fromUtf8("actionStart"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/icons/icons/start.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionStart->setIcon(icon1);
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName(QString::fromUtf8("actionExit"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/icons/icons/exit.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionExit->setIcon(icon2);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        file_Edit = new QLineEdit(centralwidget);
        file_Edit->setObjectName(QString::fromUtf8("file_Edit"));
        file_Edit->setGeometry(QRect(180, 20, 231, 25));
        descriptor_Edit = new QLineEdit(centralwidget);
        descriptor_Edit->setObjectName(QString::fromUtf8("descriptor_Edit"));
        descriptor_Edit->setGeometry(QRect(180, 60, 231, 25));
        path_Edit = new QLineEdit(centralwidget);
        path_Edit->setObjectName(QString::fromUtf8("path_Edit"));
        path_Edit->setGeometry(QRect(180, 140, 231, 25));
        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 20, 64, 17));
        label_2 = new QLabel(centralwidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(10, 60, 161, 17));
        label_3 = new QLabel(centralwidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(10, 140, 141, 17));
        descriptor_Edit_2 = new QLineEdit(centralwidget);
        descriptor_Edit_2->setObjectName(QString::fromUtf8("descriptor_Edit_2"));
        descriptor_Edit_2->setGeometry(QRect(180, 100, 231, 25));
        label_4 = new QLabel(centralwidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(10, 100, 151, 17));
        Start_Button = new QPushButton(centralwidget);
        Start_Button->setObjectName(QString::fromUtf8("Start_Button"));
        Start_Button->setGeometry(QRect(10, 180, 121, 41));
        Start_Button_2 = new QPushButton(centralwidget);
        Start_Button_2->setObjectName(QString::fromUtf8("Start_Button_2"));
        Start_Button_2->setGeometry(QRect(150, 180, 121, 41));
        exit_Button = new QPushButton(centralwidget);
        exit_Button->setObjectName(QString::fromUtf8("exit_Button"));
        exit_Button->setGeometry(QRect(300, 180, 111, 41));
        MainWindow->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 434, 21));
        menuAbout = new QMenu(menubar);
        menuAbout->setObjectName(QString::fromUtf8("menuAbout"));
        MainWindow->setMenuBar(menubar);

        menubar->addAction(menuAbout->menuAction());
        menuAbout->addAction(actionStart);
        menuAbout->addAction(aboutM);
        menuAbout->addAction(actionExit);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
        aboutM->setText(QApplication::translate("MainWindow", "about", 0, QApplication::UnicodeUTF8));
        aboutM->setShortcut(QApplication::translate("MainWindow", "F1", 0, QApplication::UnicodeUTF8));
        actionStart->setText(QApplication::translate("MainWindow", "open", 0, QApplication::UnicodeUTF8));
        actionStart->setShortcut(QApplication::translate("MainWindow", "F3", 0, QApplication::UnicodeUTF8));
        actionExit->setText(QApplication::translate("MainWindow", "exit", 0, QApplication::UnicodeUTF8));
        actionExit->setShortcut(QApplication::translate("MainWindow", "F5", 0, QApplication::UnicodeUTF8));
        file_Edit->setText(QApplication::translate("MainWindow", "DS10.p3", 0, QApplication::UnicodeUTF8));
        descriptor_Edit->setText(QApplication::translate("MainWindow", "DS10", 0, QApplication::UnicodeUTF8));
        path_Edit->setText(QApplication::translate("MainWindow", "./", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MainWindow", "*.p3 file", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MainWindow", "current descriptor name", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("MainWindow", "path to the file", 0, QApplication::UnicodeUTF8));
        descriptor_Edit_2->setText(QApplication::translate("MainWindow", "DS10", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("MainWindow", "new descriptor name", 0, QApplication::UnicodeUTF8));
        Start_Button->setText(QApplication::translate("MainWindow", "Restore", 0, QApplication::UnicodeUTF8));
        Start_Button_2->setText(QApplication::translate("MainWindow", "Rename", 0, QApplication::UnicodeUTF8));
        exit_Button->setText(QApplication::translate("MainWindow", "Exit", 0, QApplication::UnicodeUTF8));
        menuAbout->setTitle(QApplication::translate("MainWindow", "Option", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
