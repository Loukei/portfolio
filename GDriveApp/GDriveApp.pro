#-------------------------------------------------
#
# Project created by QtCreator 2019-10-14T11:35:08
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GDriveApp
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        gdriveabout.cpp \
        gdriveaboutresource.cpp \
        gdrivefiledownloaddialog.cpp \
        gdrivefiledownloader.cpp \
        gdrivefilemultipartcreate.cpp \
        gdrivefileresource.cpp \
        gdrivefileresumablecreate.cpp \
        gdrivefilesimplecreate.cpp \
        gdrivefiletask.cpp \
        googledriveservice.cpp \
        main.cpp \
        mainwindow.cpp

HEADERS += \
        gdriveabout.h \
        gdriveaboutresource.h \
        gdrivefiledownloaddialog.h \
        gdrivefiledownloader.h \
        gdrivefilemultipartcreate.h \
        gdrivefileresource.h \
        gdrivefileresumablecreate.h \
        gdrivefilesimplecreate.h \
        gdrivefiletask.h \
        googledriveservice.h \
        mainwindow.h \
        oauthglobal.h

FORMS += \
        gdrivefiledownloaddialog.ui \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

## add Oauth network
QT += network networkauth
