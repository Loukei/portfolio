#include "recentfiles.h"
#include <QSettings>

RecentFiles::RecentFiles()
{
}

RecentFiles::~RecentFiles()
{
}

bool RecentFiles::readSettings(QSettings *settings)
{
    if(settings)
    {
        const int size = settings->beginReadArray(recentFilesKey());
        for (int i=0;i<size;++i) {
            settings->setArrayIndex(i);
            this->append(settings->value(fileKey()).toString());
        }
        settings->endArray();
        return true;
    }else {
        qDebug("RecentFiles::readSettings [Error] No settings.");
        return false;
    }
}

bool RecentFiles::writeSettings(QSettings *settings)
{
    if(settings)
    {
        settings->beginWriteArray(recentFilesKey());
        for (int i=0; i<this->size(); ++i) {
            settings->setArrayIndex(i);
            settings->setValue(fileKey(),this->at(i));
        }
        settings->endArray();
        return true;
    }else {
        qDebug("RecentFiles::writeSettings() [Error]  No settings.");
        return false;
    }
}

bool RecentFiles::prepend(const QString &filename)
{
    if(filename.isEmpty()){
        //qDebug("RecentFiles::prepand() [Warring] Empty file path.");
        return false;
    }

    QStringList::removeAll(filename);
    QStringList::prepend(filename);
    return true;
}
