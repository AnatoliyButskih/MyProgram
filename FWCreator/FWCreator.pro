#-------------------------------------------------
#
# Project created by QtCreator 2011-02-08T13:53:09
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = FWCreator
TEMPLATE = app
CONFIG += console
CONFIG += static

greaterThan(QT_MAJOR_VERSION, 4) {
  QT += widgets
  DEFINES += HAVE_QT5
}

MOC_DIR     = tmp
OBJECTS_DIR = tmp
UI_DIR      = tmp

INCLUDEPATH += tmp

SOURCES += main.cpp\
    CommandLineParams.cpp \
    info.cpp \
    mainwindow.cpp \
    zipcompressor.cpp \
    fileitem.cpp \
    filesystemmodel.cpp \
    confirmlistmodel.cpp \
    tableview.cpp \
    fwdata.cpp \
    fwwriter.cpp \
    fwreader.cpp \
    zipfileitem.cpp \
    ghostchiper.cpp \
    tempfile.cpp \
    ipaddresslist.cpp \
    ipaddressmodel.cpp \
    ipaddressdelegate.cpp

HEADERS  += mainwindow.h \
    CommandLineParams.h \
    info.h \
    zipcompressor.h \
    fileitem.h \
    filesystemmodel.h \
    confirmlistmodel.h \
    tableview.h \
    fwdata.h \
    fwwriter.h \
    fwreader.h \
    zipfileitem.h \
    ghostchiper.h \
    tempfile.h \
    ipaddresslist.h \
    ipaddressmodel.h \
    ipaddressdelegate.h

unix:LIBS+=-lz
win32:RC_FILE = fwcreator.rc

win32:INCLUDEPATH+=../3DPartyLib/zlib-1.2.8/include
win32:INCLUDEPATH+=../3DPartyLib/libssh-0.6.3_win_fix/include

CONFIG(release, debug|release) {
win32:LIBS+=../3DPartyLib/zlib-1.2.8/release/zlib.lib
win32:LIBS+=../3DPartyLib/libssh-0.6.3_win_fix/release/ssh.lib
win32:LIBS+=../3DPartyLib/openssl-1.0.1j/release/ssleay32.lib
win32:LIBS+=../3DPartyLib/openssl-1.0.1j/release/libeay32.lib
} else {
win32:LIBS+=../3DPartyLib/zlib-1.2.8/debug/zlibd.lib
win32:LIBS+=../3DPartyLib/libssh-0.6.3_win_fix/debug/ssh.lib
win32:LIBS+=../3DPartyLib/openssl-1.0.1j/debug/ssleay32.lib
win32:LIBS+=../3DPartyLib/openssl-1.0.1j/debug/libeay32.lib
}

TRANSLATIONS = fwcreator_ru.ts
RESOURCES += \
    resources.qrc

FORMS += \
    mainwindow.ui

DISTFILES += \
    key
