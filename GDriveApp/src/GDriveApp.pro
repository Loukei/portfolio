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
        GDriveLib/gdriveabout.cpp \
        GDriveLib/gdriveaboutresource.cpp \
        GDriveLib/gdrivefiledownloader.cpp \
        GDriveLib/gdrivefilemultipartcreate.cpp \
        GDriveLib/gdrivefileresource.cpp \
        GDriveLib/gdrivefileresumablecreate.cpp \
        GDriveLib/gdrivefiles.cpp \
        GDriveLib/gdrivefilesimplecreate.cpp \
        GDriveLib/gdrivefiletask.cpp \
        GDriveLib/googledriveservice.cpp \
        main.cpp \
        mainwindow.cpp \
        uidownloaddialog.cpp

HEADERS += \
        GDriveLib/gdriveabout.h \
        GDriveLib/gdriveaboutresource.h \
        GDriveLib/gdrivefiledownloader.h \
        GDriveLib/gdrivefilemultipartcreate.h \
        GDriveLib/gdrivefileresource.h \
        GDriveLib/gdrivefileresumablecreate.h \
        GDriveLib/gdrivefiles.h \
        GDriveLib/gdrivefilesimplecreate.h \
        GDriveLib/gdrivefiletask.h \
        GDriveLib/googledriveservice.h \
        Secret/oauthglobal.h \
        mainwindow.h \
        uidownloaddialog.h

FORMS += \
        mainwindow.ui \
        uidownloaddialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

## add Oauth network
QT += network networkauth

RESOURCES += \
    resources.qrc
