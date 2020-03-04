#include "gdriveaboutresource.h"
#include <QJsonParseError>
#include <QJsonObject>
#include <QDebug>

GDrive::GDriveAboutResource::GDriveAboutResource(const QByteArray &data)
{
    QJsonParseError jsonErr;
    //! m_data return null if parse error
    m_data = QJsonDocument::fromJson(data,&jsonErr);
    if(jsonErr.error != QJsonParseError::NoError){
        m_errorString = jsonErr.errorString();
    }
    qDebug() << "GDriveAboutResource(const QByteArray &data):" << this;
}

//GDrive::GDriveAboutResource::GDriveAboutResource(const GDrive::GDriveAboutResource &resource)
//    :m_data(resource.m_data),m_errorString(resource.m_errorString)
//{
//}

//GDrive::GDriveAboutResource::GDriveAboutResource(GDrive::GDriveAboutResource &&resource)
//    :m_data(resource.m_data),m_errorString(resource.m_errorString)
//{
//}

GDrive::GDriveAboutResource::~GDriveAboutResource()
{
    qInfo() << "GDrive::GDriveAboutResource::~GDriveAboutResource():" << this;
}

QString GDrive::GDriveAboutResource::displayName() const
{
    auto userobj = m_data.object()["user"].toObject();
    return userobj["displayName"].toString();
}

QString GDrive::GDriveAboutResource::emailAddress() const
{
    auto userobj = m_data.object()["user"].toObject();
    return userobj["emailAddress"].toString();
}

QString GDrive::GDriveAboutResource::permissionId() const
{
    auto userobj = m_data.object()["user"].toObject();
    return userobj["permissionId"].toString();
}

bool GDrive::GDriveAboutResource::isNull()
{
    return m_data.isNull();
}

QString GDrive::GDriveAboutResource::errorString() const
{
    return m_errorString;
}
