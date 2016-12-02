#-------------------------------------------------
#
# Project created by QtCreator 2016-11-23T18:44:42
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SFTPTool
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    sftpconnector.cpp \
    sftpwindow.cpp \
    sftpfilemanager.cpp \
    sftpsession.cpp \
    sftpconfigmanager.cpp \
    sftpuploaddialog.cpp \
    sftpbackupdialog.cpp

HEADERS  += mainwindow.h \
    sftpwindow.h \
    sftpconnector.h \
    sftpfilemanager.h \
    sftpsession.h \
    sftpconfigmanager.h \
    sftpuploaddialog.h \
    sftpbackupdialog.h

FORMS    += mainwindow.ui

RESOURCES += \
    res.qrc

LIBS += -L$$PWD/libssh2/src/.libs/ -lssh2 -lwsock32
LIBS += -L$$PWD/jsoncpp/libs/mingw -ljson_mingw_libmt

INCLUDEPATH += $$PWD/libssh2/include
INCLUDEPATH += $$PWD/jsoncpp/include

DISTFILES +=

RC_FILE = \
    SFTPTool.rc
