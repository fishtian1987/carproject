#-------------------------------------------------
#
# Project created by QtCreator 2017-12-18T15:54:12
#
#-------------------------------------------------

QT       += core gui sql serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = robot
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    dlg/magdriftangledlg.cpp \
    dlg/locationdriftdlg.cpp \
    dlg/piddlog.cpp \
    dlg/remotespeeddlog.cpp \
    dlg/navigationspeeddlog.cpp \
    utils/usartdriver.cpp \
    db/databasehelper.cpp \
    info/routeinfo.cpp \
    db/dataprotocol.cpp \
    dlg/collectcoordinatedlog.cpp \
    utils/transactionthread.cpp \
    dlg/autonavigationdlg.cpp \
    utils/utils.cpp \
    dlg/hintdialog.cpp \
    db/package.cpp \
    info/message.cpp \
    info/atk.cpp \
    com/mylineedit.cpp \
    com/myspinbox.cpp \
    dlg/lookdatabasedlg.cpp \
    dlg/editdatabasedlg.cpp \
    dlg/detectionspeeddlg.cpp \
    info/edges.cpp \
    info/vertexweight.cpp


HEADERS  += mainwindow.h \
    dlg/magdriftangledlg.h \
    dlg/locationdriftdlg.h \
    dlg/piddlog.h \
    dlg/remotespeeddlog.h \
    dlg/navigationspeeddlog.h \
    utils/usartdriver.h \
    db/databasehelper.h \
    info/routeinfo.h \
    db/dataprotocol.h \
    dlg/collectcoordinatedlog.h \
    utils/transactionthread.h \
    dlg/autonavigationdlg.h \
    utils/utils.h \
    dlg/hintdialog.h \
    db/package.h \
    info/message.h \
    info/atk.h \
    com/mylineedit.h \
    com/myspinbox.h \
    dlg/lookdatabasedlg.h \
    dlg/editdatabasedlg.h \
    dlg/detectionspeeddlg.h \
    info/edges.h \
    info/vertexweight.h

FORMS    += mainwindow.ui \
    win/autonavigationdlg.ui \
    win/hintdialog.ui \
    win/lookdatabasedlg.ui \
    win/detectionspeeddlg.ui

RESOURCES += \
    res.qrc
