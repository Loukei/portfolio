#include "gdriveabout.h"
#include <QOAuth2AuthorizationCodeFlow>
#include <QUrl>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QEventLoop>

using namespace GDrive;

GDriveAbout::GDriveAbout(QOAuth2AuthorizationCodeFlow *parent, AboutArgs args)
    :QObject(parent),m_data(QByteArray())
{
    //! send request to get data member, use signal received(true) to get data
    QNetworkReply *reply = parent->get(aboutArgToUrl(args));
    connect(reply,&QNetworkReply::finished,
            this,&GDriveAbout::onReplyFinished);
    connect(reply,QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this,&GDriveAbout::onReplyError);
}

GDriveAbout::GDriveAbout(QOAuth2AuthorizationCodeFlow *parent, const QString &fields)
{
    QNetworkReply *reply = parent->get(fields);
    connect(reply,&QNetworkReply::finished,
            this,&GDriveAbout::onReplyFinished);
    connect(reply,QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this,&GDriveAbout::onReplyError);
}

GDriveAbout::~GDriveAbout()
{
}

QString GDriveAbout::errorString() const
{
    return m_errStr;
}

GDriveAboutResource GDriveAbout::getResource() const
{
    QJsonParseError jsonErr;
    //! doc return null if parse error
    QJsonDocument doc = QJsonDocument::fromJson(m_data,&jsonErr);
    if(jsonErr.error != QJsonParseError::NoError){
        qWarning() << Q_FUNC_INFO << jsonErr.errorString();
        return GDriveAboutResource();
    }
    return GDriveAboutResource(doc);// RVO will work
}

QByteArray GDriveAbout::getReplyString() const
{
    return m_data;
}

QUrl GDriveAbout::aboutArgToUrl(const AboutArgs& args) const
{
    QString userargs;
    switch (args) {
    case 0x01: //DISPLAYNAME
        userargs = QString("user(displayName)");
        break;
    case 0x02: //EMAILADDRESS
        userargs = QString("user(emailAddress)");
        break;
    case 0x04: //PERMISSIONID
        userargs = QString("user(permissionId)");
        break;
    case 0x03 : //DISPLAYNAME|EMAILADDRESS
        userargs = QString("user(displayName,emailAddress)");
        break;
    case 0x06: //EMAILADDRESS|PERMISSIONID
        userargs = QString("user(emailAddress,permissionId)");
        break;
    case 0x05: //DISPLAYNAME|PERMISSIONID
        userargs = QString("user(displayName,permissionId)");
        break;
    case 0x07: //DISPLAYNAME|EMAILADDRESS|PERMISSIONID
        userargs = QString("user(displayName,emailAddress,permissionId)");
        break;
    default:   //DISPLAYNAME|EMAILADDRESS|PERMISSIONID
        userargs = QString("user(displayName,emailAddress,permissionId)");
        break;
    }
    return QUrl("https://www.googleapis.com/drive/v3/about?fields=" + userargs);
}

void GDriveAbout::onReplyFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if(reply->error() == QNetworkReply::NoError){
        m_data = reply->readAll();
        emit received(true);
    }
    reply->deleteLater();
}

void GDriveAbout::onReplyError(QNetworkReply::NetworkError)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    qWarning() << "[Error]About reply network error: " << reply->error();
    m_data = reply->readAll();
    emit received(false);
    reply->deleteLater();
}
