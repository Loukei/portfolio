#include "gdrivefilelist.h"
#include <QJsonParseError>
#include <QJsonObject>

GDrive::GDriveFileList::GDriveFileList(const QJsonDocument &doc)
{
    parseDocument(doc);
}

GDrive::GDriveFileList::GDriveFileList()
{
    m_kind = QString();
    m_nextPageToken = QString();
    m_incompleteSearch = false;
    m_filelist = QJsonArray();
}

GDrive::GDriveFileList::~GDriveFileList()
{
}

QString GDrive::GDriveFileList::kind() const
{
    return m_kind;
}

QString GDrive::GDriveFileList::nextPageToken() const
{
    return m_nextPageToken;
}

bool GDrive::GDriveFileList::incompleteSearch() const
{
    return m_incompleteSearch;
}

QJsonArray GDrive::GDriveFileList::files() const
{
    return m_filelist;
}

void GDrive::GDriveFileList::parseDocument(const QJsonDocument &doc)
{
    QJsonObject obj = doc.object();
    m_kind = obj["kind"].toString();
    m_nextPageToken = obj["nextPageToken"].toString();
    m_incompleteSearch = obj["incompleteSearch"].toBool();
    m_filelist = obj["files"].toArray();
}
