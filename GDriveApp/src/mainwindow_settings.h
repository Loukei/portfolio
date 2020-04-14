#ifndef MAINWINDOW_SETTINGS_H
#define MAINWINDOW_SETTINGS_H
#include <QSettings>
#include <QRect>
/*!
 * \namespace Settings
 * \brief Settings is used for collect all settings key and read settings
 */
namespace Settings {
const QString key_Geometry = "MainWindow/Geometry";
const QString key_Upload_FilePath = "MainWindow/DialogUpload/FilePath";
const QString key_Download_FilePath = "MainWindow/DialogDownload/FilePath";
const QString key_Download_FileID = "MainWindow/DialogDownload/FileID";
const QString key_FileGet_FileID = "MainWindow/DialogFileMataData/FileID";
const QString key_FileGet_Fields = "MainWindow/DialogFileMataData/Fields";
const QString key_Update_FilePath = "MainWindow/DialogUpdate/FilePath";
const QString key_Update_FileID = "MainWindow/DialogUpdate/FileID";

inline QRect MainWindow_Geometry(QSettings *settings)
{
    return settings->value(key_Geometry,QRect(0,0,630,495)).toRect();
}

inline QString Upload_FilePath(QSettings *settings)
{
    return settings->value(key_Upload_FilePath,"/home").toString();
}

inline QString Download_FilePath(QSettings *settings)
{
    return settings->value(key_Download_FilePath,"/home").toString();
}

inline QString Download_FileID(QSettings *settings)
{
    return settings->value(key_Download_FileID,"YOUR_FILE_ID").toString();
}

inline QString FileGet_FileID(QSettings *settings)
{
    return settings->value(key_FileGet_FileID,"YOUR_FILE_ID").toString();
}

inline QString FileGet_Fields(QSettings *settings)
{
    return settings->value(key_FileGet_Fields,"YOUR_FILE_ID").toString();
}

inline QString Update_FilePath(QSettings *settings)
{
    return settings->value(key_Update_FilePath,"/home").toString();
}

inline QString Update_FileID(QSettings *settings)
{
    return settings->value(key_Update_FileID,"YOUR_FILE_ID").toString();
}
}

#endif // MAINWINDOW_SETTINGS_H
