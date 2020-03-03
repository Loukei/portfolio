#include "gdrivefileresource.h"
#include <QJsonParseError>
#include <QJsonObject>
#include <QStringList>
#include <QJsonArray>

GDrive::GDriveFileResource::GDriveFileResource(const QByteArray &data)
{
    QJsonParseError jsonErr;
    //! m_data return null if parse error
    m_data = QJsonDocument::fromJson(data,&jsonErr);
    if(jsonErr.error != QJsonParseError::NoError){
        m_errorString = jsonErr.errorString();
        return;
    }
}

//GDrive::GDriveFileResource::GDriveFileResource(GDrive::GDriveFileResource &&resource)
//    :m_data(resource.m_data),m_errorString(resource.m_errorString)
//{
//}

GDrive::GDriveFileResource::~GDriveFileResource()
{
}

QString GDrive::GDriveFileResource::kind() const
{
    QJsonObject obj = m_data.object();
    return obj["kind"].toString();
}

QString GDrive::GDriveFileResource::fileID() const
{
    QJsonObject obj = m_data.object();
    return obj["id"].toString();
}

QString GDrive::GDriveFileResource::name() const
{
    QJsonObject obj = m_data.object();
    return obj["name"].toString();
}

QString GDrive::GDriveFileResource::mimeType() const
{
    QJsonObject obj = m_data.object();
    return obj["mimeType"].toString();
}

QString GDrive::GDriveFileResource::description() const
{
    QJsonObject obj = m_data.object();
    return obj["description"].toString();
}

QStringList GDrive::GDriveFileResource::parents() const
{
    //! todo
    QJsonObject obj = m_data.object();
    QJsonArray arr = obj["parents"].toArray();
    return QStringList();
}

long GDrive::GDriveFileResource::size() const
{    
    QJsonObject obj = m_data.object();
    return obj["size"].toInt();
}

bool GDrive::GDriveFileResource::isNull() const
{
    return m_data.isNull();
}

QString GDrive::GDriveFileResource::errorString() const
{
    return m_errorString;
}
