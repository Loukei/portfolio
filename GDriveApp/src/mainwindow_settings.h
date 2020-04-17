#ifndef MAINWINDOW_SETTINGS_H
#define MAINWINDOW_SETTINGS_H
#include <QSettings>
#include <QRect>
/*!
 * \namespace Settings
 * \brief Settings is used for collect all settings key and read settings
 */
namespace Settings {
const QString key_Geometry = QStringLiteral("MainWindow/Geometry");
const QString key_Upload_FilePath = QStringLiteral("MainWindow/DialogUpload/FilePath");
const QString key_Download_FilePath = QStringLiteral("MainWindow/DialogDownload/FilePath");
const QString key_Download_FileID = QStringLiteral("MainWindow/DialogDownload/FileID");
const QString key_FileGet_FileID = QStringLiteral("MainWindow/DialogFileMataData/FileID");
const QString key_FileGet_Fields = QStringLiteral("MainWindow/DialogFileMataData/Fields");
const QString key_Update_FilePath = QStringLiteral("MainWindow/DialogUpdate/FilePath");
const QString key_Update_FileID = QStringLiteral("MainWindow/DialogUpdate/FileID");
const QString key_OAuth_CurrentToken = QStringLiteral("MainWindow/OAuth/token");

inline QRect MainWindow_Geometry(QSettings *settings)
{
    return settings->value(key_Geometry,QRect(0,0,630,495)).toRect();
}

inline QString Upload_FilePath(QSettings *settings)
{
    return settings->value(key_Upload_FilePath,QStringLiteral("/home")).toString();
}

inline QString Download_FilePath(QSettings *settings)
{
    return settings->value(key_Download_FilePath,QStringLiteral("/home")).toString();
}

inline QString Download_FileID(QSettings *settings)
{
    return settings->value(key_Download_FileID,QStringLiteral("YOUR_FILE_ID")).toString();
}

inline QString FileGet_FileID(QSettings *settings)
{
    return settings->value(key_FileGet_FileID,QStringLiteral("YOUR_FILE_ID")).toString();
}

inline QString FileGet_Fields(QSettings *settings)
{
    return settings->value(key_FileGet_Fields,QStringLiteral("YOUR_FILE_ID")).toString();
}

inline QString Update_FilePath(QSettings *settings)
{
    return settings->value(key_Update_FilePath,QStringLiteral("/home")).toString();
}

inline QString Update_FileID(QSettings *settings)
{
    return settings->value(key_Update_FileID,QStringLiteral("YOUR_FILE_ID")).toString();
}

inline QString OAuth_CurrentToken(QSettings *settings){
    return settings->value(key_OAuth_CurrentToken,QString()).toString();
}
}

#endif // MAINWINDOW_SETTINGS_H
