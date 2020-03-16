#ifndef RECENTFILES_H
#define RECENTFILES_H
#include <QStringList>
class QSettings;

class RecentFiles : public QStringList
{
public:
    RecentFiles();
    ~RecentFiles();

    bool readSettings(QSettings *settings);
    bool writeSettings(QSettings *settings);
    bool prepend(const QString &filename);
    inline static QString recentFilesKey(){return QStringLiteral("RecentFileList");}
    inline static QString fileKey(){return QStringLiteral("filePath");}
};

#endif // RECENTFILES_H

//! RecentFiles 類別繼承自QStringList，但是提供了讀寫QSetting的功能，
//! 以及保存了recent file在Settings裡的key值
//! prepend函數則保證每個寫入RecentFiles的資料一定不會重複並堆疊在最上方
