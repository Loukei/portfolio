#include "gdrivefileresource.h"
#include <QJsonParseError>
#include <QJsonObject>
#include <QStringList>
#include <QJsonArray>

//GDrive::GDriveFileResource::GDriveFileResource(const QByteArray &data)
//{
//    QJsonParseError jsonErr;
//    //! m_data return null if parse error
//    m_data = QJsonDocument::fromJson(data,&jsonErr);
//    if(jsonErr.error != QJsonParseError::NoError){
//        m_errorString = jsonErr.errorString();
//        return;
//    }
//}

GDrive::GDriveFileResource::GDriveFileResource(const QJsonValue &value)
    :m_object(value.toObject())
{
}

GDrive::GDriveFileResource::GDriveFileResource(const QJsonDocument &doc)
    :m_object(doc.object())
{
}

GDrive::GDriveFileResource::GDriveFileResource()
    :m_object()
{
}

GDrive::GDriveFileResource::~GDriveFileResource()
{
}

QString GDrive::GDriveFileResource::kind() const
{
    return m_object["kind"].toString();
}

QString GDrive::GDriveFileResource::id() const
{
    return m_object["id"].toString();
}

QString GDrive::GDriveFileResource::name() const
{
    return m_object["name"].toString();
}

QString GDrive::GDriveFileResource::mimeType() const
{
    return m_object["mimeType"].toString();
}

QString GDrive::GDriveFileResource::description() const
{
    return m_object["description"].toString();
}

long GDrive::GDriveFileResource::version() const
{
    return m_object["version"].toInt();
}

long GDrive::GDriveFileResource::size() const
{    
    return m_object["size"].toInt();
}

bool GDrive::GDriveFileResource::isEmpty() const
{
    return m_object.isEmpty();
}
