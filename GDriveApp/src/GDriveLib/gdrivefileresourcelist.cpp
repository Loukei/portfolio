#include "gdrivefileresourcelist.h"
#include <QJsonParseError>
#include <QJsonObject>

GDrive::GDriveFileResourceList::GDriveFileResourceList(const QJsonDocument &doc)
{
    parseDocument(doc);
}

GDrive::GDriveFileResourceList::GDriveFileResourceList()
{
    m_kind = QString();
    m_nextPageToken = QString();
    m_incompleteSearch = false;
    m_filelist = QJsonArray();
}

GDrive::GDriveFileResourceList::~GDriveFileResourceList()
{
}

QString GDrive::GDriveFileResourceList::kind() const
{
    return m_kind;
}

QString GDrive::GDriveFileResourceList::nextPageToken() const
{
    return m_nextPageToken;
}

bool GDrive::GDriveFileResourceList::incompleteSearch() const
{
    return m_incompleteSearch;
}

QJsonArray GDrive::GDriveFileResourceList::files() const
{
    return m_filelist;
}

void GDrive::GDriveFileResourceList::parseDocument(const QJsonDocument &doc)
{
    QJsonObject obj = doc.object();
    m_kind = obj["kind"].toString();
    m_nextPageToken = obj["nextPageToken"].toString();
    m_incompleteSearch = obj["incompleteSearch"].toBool();
    m_filelist = obj["files"].toArray();
}
