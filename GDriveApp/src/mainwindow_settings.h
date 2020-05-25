#ifndef MAINWINDOW_SETTINGS_H
#define MAINWINDOW_SETTINGS_H
#include <QSettings>
#include <QRect>
/*!
 * \namespace Settings
 * \brief Settings is used for collect all settings key and read settings
 */
namespace Settings
{
const QString key_AppVersion = QStringLiteral("App/Version");
const QString key_Geometry = QStringLiteral("MainWindow/Geometry");
const QString key_Upload_FilePath = QStringLiteral("MainWindow/DialogUpload/FilePath");
const QString key_Download_FilePath = QStringLiteral("MainWindow/DialogDownload/FilePath");
const QString key_Download_FileID = QStringLiteral("MainWindow/DialogDownload/FileID");
const QString key_FileGet_FileID = QStringLiteral("MainWindow/DialogFileMataData/FileID");
const QString key_FileGet_Fields = QStringLiteral("MainWindow/DialogFileMataData/Fields");
const QString key_Update_FilePath = QStringLiteral("MainWindow/DialogUpdate/FilePath");
const QString key_Update_FileID = QStringLiteral("MainWindow/DialogUpdate/FileID");
const QString key_OAuth_UserEmail = QStringLiteral("OAuth/UserEmail");
const QString key_OAuth_IsRememberMe = QStringLiteral("OAuth/IsRememberMe");
const QString key_OAuth_RefreshToken = QStringLiteral("OAuth/RefreshToken");

/*Settings reader function*/
inline QString App_Version(const QSettings *settings){
    return settings->value(key_AppVersion,QStringLiteral("0.0.0")).toString();
}

inline QRect MainWindow_Geometry(const QSettings *settings){
    return settings->value(key_Geometry,QRect(0,0,630,495)).toRect();
}

inline QString Upload_FilePath(const QSettings *settings){
    return settings->value(key_Upload_FilePath,QStringLiteral("/home")).toString();
}

inline QString Download_FilePath(const QSettings *settings){
    return settings->value(key_Download_FilePath,QStringLiteral("/home")).toString();
}

inline QString Download_FileID(const QSettings *settings){
    return settings->value(key_Download_FileID,QStringLiteral("YOUR_FILE_ID")).toString();
}

inline QString FileGet_FileID(const QSettings *settings){
    return settings->value(key_FileGet_FileID,QStringLiteral("YOUR_FILE_ID")).toString();
}

inline QString FileGet_Fields(const QSettings *settings){
    return settings->value(key_FileGet_Fields,QStringLiteral("YOUR_FILE_ID")).toString();
}

inline QString Update_FilePath(const QSettings *settings){
    return settings->value(key_Update_FilePath,QStringLiteral("/home")).toString();
}

inline QString Update_FileID(const QSettings *settings){
    return settings->value(key_Update_FileID,QStringLiteral("YOUR_FILE_ID")).toString();
}

inline QString OAuth_UserID(const QSettings *settings){
    return settings->value(key_OAuth_UserEmail,QString()).toString();
}

inline QString OAuth_RefreshToken(const QSettings *settings){
    return settings->value(key_OAuth_RefreshToken,QString()).toString();
}

inline bool OAuth_IsRememberMe(const QSettings *settings){
    return settings->value(key_OAuth_IsRememberMe,QVariant(false)).toBool();
}
} // Settings

#endif // MAINWINDOW_SETTINGS_H
