#include "gdriveabout.h"
#include <QOAuth2AuthorizationCodeFlow>
#include <QUrl>
#include <QUrlQuery>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QEventLoop>

using namespace GDrive;

GDriveAbout::GDriveAbout(QOAuth2AuthorizationCodeFlow *parent, const QString &fields)
{
    QNetworkReply *reply = parent->get(buildRequestUrl(fields));
    connect(reply,&QNetworkReply::finished,
            this,&GDriveAbout::onReplyFinished);
    connect(reply,QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this,&GDriveAbout::onReplyError);
}

GDriveAbout::~GDriveAbout()
{
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

bool GDriveAbout::isComplete() const
{
    return m_isComplete;
}

bool GDriveAbout::isFailed() const
{
    return m_isFailed;
}

QUrl GDriveAbout::buildRequestUrl(const QString &field) const
{
    QUrl url(key_AboutUrl);
    QUrlQuery query;
    query.addQueryItem("fields",field);
    url.setQuery(query);
    return url;
}

void GDriveAbout::onReplyFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if(reply->error() == QNetworkReply::NoError){
        m_data = reply->readAll();
        m_isComplete = true;
        m_isFailed = false;
        emit finished();
    }
    reply->deleteLater();
}

void GDriveAbout::onReplyError(QNetworkReply::NetworkError)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    qWarning() << "[Error]About reply network error: " << reply->error();
    m_data = reply->readAll();
    m_isComplete = true;
    m_isFailed = true;
    emit finished();
    reply->deleteLater();
}
