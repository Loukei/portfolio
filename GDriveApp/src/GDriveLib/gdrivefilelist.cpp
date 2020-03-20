#include "gdrivefilelist.h"
#include <QJsonParseError>
#include <QJsonObject>

GDrive::GDriveFileList::GDriveFileList(const QByteArray &data)
{
    QJsonParseError jsonErr;
    //! m_data return null if parse error
    QJsonDocument doc = QJsonDocument::fromJson(data,&jsonErr);
    if(jsonErr.error != QJsonParseError::NoError){
        m_errorString = jsonErr.errorString();
        return;
    }
    //! Parse doc here
    parseDocument(doc);
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

QList<GDrive::GDriveFileResource> GDrive::GDriveFileList::files() const
{
    QList<GDrive::GDriveFileResource> list;
    for (int i = 0; i < m_filelist.size(); i++) {
        list.append(GDriveFileResource(m_filelist.at(i)));
    }
    return list;
}

GDrive::GDriveFileResource GDrive::GDriveFileList::at(int index)
{
    QJsonValue value = m_filelist.at(index);
    if(value.isUndefined() | value.isNull()){
        return GDriveFileResource(); //! return empty GDriveFileResource
    }else{
       return GDriveFileResource(value);
    }
}

int GDrive::GDriveFileList::size() const
{
    return m_filelist.size();
}

QString GDrive::GDriveFileList::errorString() const
{
    return m_errorString;
}

void GDrive::GDriveFileList::parseDocument(const QJsonDocument &doc)
{
    QJsonObject obj = doc.object();
    m_kind = obj["kind"].toString();
    m_nextPageToken = obj["nextPageToken"].toString();
    m_incompleteSearch = obj["incompleteSearch"].toBool();
    m_filelist = obj["files"].toArray();
}
