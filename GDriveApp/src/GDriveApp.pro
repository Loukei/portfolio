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
        Ecrypt/simplecrypt.cpp \
        GDriveLib/gdriveabout.cpp \
        GDriveLib/gdriveaboutresource.cpp \
        GDriveLib/gdriveerrorformat.cpp \
        GDriveLib/gdrivefiledownloader.cpp \
        GDriveLib/gdrivefileget.cpp \
        GDriveLib/gdrivefilemultipartcreate.cpp \
        GDriveLib/gdrivefilemultipartupdate.cpp \
        GDriveLib/gdrivefileresource.cpp \
        GDriveLib/gdrivefileresourcelist.cpp \
        GDriveLib/gdrivefileresumablecreate.cpp \
        GDriveLib/gdrivefileresumableupdate.cpp \
        GDriveLib/gdrivefilesearch.cpp \
        GDriveLib/gdrivefilesimplecreate.cpp \
        GDriveLib/gdrivefilesimpleupdate.cpp \
        GDriveLib/gdrivefiletask.cpp \
        GDriveLib/googledriveservice.cpp \
        QJsonModel/qjsonmodel.cpp \
        downloaddialog.cpp \
        filematadatadialog.cpp \
        main.cpp \
        mainwindow.cpp \
        networkimgloader.cpp \
        searchdialog.cpp \
        updatedialog.cpp \
        uploaddialog.cpp

HEADERS += \
        Ecrypt/simplecrypt.h \
        GDriveLib/gdriveabout.h \
        GDriveLib/gdriveaboutresource.h \
        GDriveLib/gdriveerrorformat.h \
        GDriveLib/gdrivefiledownloader.h \
        GDriveLib/gdrivefileget.h \
        GDriveLib/gdrivefilemultipartcreate.h \
        GDriveLib/gdrivefilemultipartupdate.h \
        GDriveLib/gdrivefileresource.h \
        GDriveLib/gdrivefileresourcelist.h \
        GDriveLib/gdrivefileresumablecreate.h \
        GDriveLib/gdrivefileresumableupdate.h \
        GDriveLib/gdrivefilesearch.h \
        GDriveLib/gdrivefilesimplecreate.h \
        GDriveLib/gdrivefilesimpleupdate.h \
        GDriveLib/gdrivefiletask.h \
        GDriveLib/googledriveservice.h \
        QJsonModel/qjsonmodel.h \
        Secret/oauthglobal.h \
        accountwidgetacts.h \
        downloaddialog.h \
        filematadatadialog.h \
        mainwindow.h \
        mainwindow_settings.h \
        networkimgloader.h \
        searchdialog.h \
        updatedialog.h \
        uploaddialog.h

FORMS += \
        downloaddialog.ui \
        filematadatadialog.ui \
        mainwindow.ui \
        searchdialog.ui \
        updatedialog.ui \
        uploaddialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

## add Oauth network
QT += network networkauth

RESOURCES += \
    resources.qrc
