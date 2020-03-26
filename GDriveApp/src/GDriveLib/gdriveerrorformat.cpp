#include "gdriveerrorformat.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

GDrive::GDriveErrorFormat::GDriveErrorFormat(const QJsonDocument &doc)
{
    m_isEmpty = doc.isEmpty();
    QJsonObject errorObj = doc.object()["error"].toObject();
    m_code = errorObj["code"].toInt();
    m_message = errorObj["message"].toString();
    m_errors = errorObj["errors"].toArray();
}

GDrive::GDriveErrorFormat::GDriveErrorFormat()
    :m_isEmpty(true),m_code(404),m_message(QString()),m_errors(QJsonArray())
{

}

int GDrive::GDriveErrorFormat::code() const
{
    return m_code;
}

QJsonArray GDrive::GDriveErrorFormat::errors() const
{
    return m_errors;
}

QJsonValue GDrive::GDriveErrorFormat::errorListAt(int index)
{
    return m_errors.at(index);
}

QString GDrive::GDriveErrorFormat::message() const
{
    return m_message;
}

bool GDrive::GDriveErrorFormat::isEmpty()
{
    return m_isEmpty;
}

GDrive::GDriveErrorItem::GDriveErrorItem(const QJsonValue &value)
{
    QJsonObject obj = value.toObject();
    m_isEmpty = obj.isEmpty();
    m_domain = obj["domain"].toString();
    m_reason = obj["reason"].toString();
    m_message = obj["message"].toString();
    m_locationType = obj["locationType"].toString();
    m_location = obj["location"].toString();
}

GDrive::GDriveErrorItem::~GDriveErrorItem()
{

}

QString GDrive::GDriveErrorItem::domain() const
{
    return m_domain;
}

QString GDrive::GDriveErrorItem::reason() const
{
    return m_reason;
}

QString GDrive::GDriveErrorItem::message() const
{
    return m_message;
}

QString GDrive::GDriveErrorItem::locationType() const
{
    return m_locationType;
}

QString GDrive::GDriveErrorItem::location() const
{
    return m_location;
}

bool GDrive::GDriveErrorItem::isEmpty() const
{
    return m_isEmpty;
}
