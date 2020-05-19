#include "gdriveaboutresource.h"
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonArray>
#include <QDebug>

//GDrive::GDriveAboutResource::GDriveAboutResource(const QByteArray &data)
//{
//    QJsonParseError jsonErr;
//    //! m_data return null if parse error
//    m_data = QJsonDocument::fromJson(data,&jsonErr);
//    if(jsonErr.error != QJsonParseError::NoError){
//        m_errorString = jsonErr.errorString();
//    }
//    qInfo() << Q_FUNC_INFO << this;
//}

GDrive::GDriveAboutResource::GDriveAboutResource(const QJsonDocument &doc)
    :m_object(doc.object())
{
}

GDrive::GDriveAboutResource::GDriveAboutResource()
{
}

GDrive::GDriveAboutResource::~GDriveAboutResource()
{
}

QString GDrive::GDriveAboutResource::kind() const
{
    return m_object["kind"].toString();
}

QJsonObject GDrive::GDriveAboutResource::user() const
{
    return m_object["user"].toObject();
}

QString GDrive::GDriveAboutResource::user_kind() const
{
    return user()["kind"].toString();
}

QString GDrive::GDriveAboutResource::user_displayName() const
{
    return user()["displayName"].toString();
}

QString GDrive::GDriveAboutResource::user_photoLink() const
{
    return user()["photoLink"].toString();
}

bool GDrive::GDriveAboutResource::user_me() const
{
    return user()["me"].toBool();
}

QString GDrive::GDriveAboutResource::user_permissionId() const
{
    return user()["permissionId"].toString();
}

QString GDrive::GDriveAboutResource::user_emailAddress() const
{
    return user()["emailAddress"].toString();
}

QJsonObject GDrive::GDriveAboutResource::storageQuota() const
{
    return m_object["storageQuota"].toObject();
}

long GDrive::GDriveAboutResource::storageQuota_limit() const
{
    return storageQuota()["limit"].toInt();
}

long GDrive::GDriveAboutResource::storageQuota_usage() const
{
    return storageQuota()["usage"].toInt();
}

long GDrive::GDriveAboutResource::storageQuota_usageInDrive() const
{
    return storageQuota()["usageInDrive"].toInt();
}

long GDrive::GDriveAboutResource::storageQuota_usageInDriveTrash() const
{
    return storageQuota()["usageInDriveTrash"].toInt();
}

QJsonObject GDrive::GDriveAboutResource::importFormats() const
{
    return m_object["importFormats"].toObject();
}

QJsonObject GDrive::GDriveAboutResource::exportFormats() const
{
    return m_object["exportFormats"].toObject();
}

QJsonObject GDrive::GDriveAboutResource::maxImportSizes() const
{
    return m_object["maxImportSizes"].toObject();
}

long GDrive::GDriveAboutResource::maxUploadSize() const
{
    return m_object["maxUploadSize"].toInt();
}

bool GDrive::GDriveAboutResource::appInstalled() const
{
    return m_object["appInstalled"].toBool();
}

QJsonArray GDrive::GDriveAboutResource::folderColorPalette() const
{
    return m_object["folderColorPalette"].toArray();
}

QJsonArray GDrive::GDriveAboutResource::driveThemes() const
{
    return m_object["teamDriveThemes"].toArray();
}

bool GDrive::GDriveAboutResource::canCreateDrives() const
{
    return m_object["canCreateDrives"].toBool();
}

bool GDrive::GDriveAboutResource::isEmpty() const
{
    return m_object.isEmpty();
}
