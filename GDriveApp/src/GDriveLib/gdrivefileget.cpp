#include "gdrivefileget.h"
#include <QOAuth2AuthorizationCodeFlow>
#include <QUrlQuery>

GDrive::GDriveFileGet::GDriveFileGet(const QString &fileId,
                                     const QString &fields,
                                     QOAuth2AuthorizationCodeFlow *parent)
    :GDriveFileTask (parent)
{
    QUrl url = buildUrl(fileId,mp_google->clientIdentifierSharedKey(),fields);
    request_FilesGet(url);
}

GDrive::GDriveFileGet::GDriveFileGet(const QString &fileId,
                                     const QUrlQuery &args,
                                     QOAuth2AuthorizationCodeFlow *parent)
    :GDriveFileTask (parent)
{
    QUrl url = buildUrl(fileId,mp_google->clientIdentifierSharedKey(),args);
    request_FilesGet(url);
}

GDrive::GDriveFileGet::~GDriveFileGet()
{

}

QByteArray GDrive::GDriveFileGet::getReplyString() const
{
    return m_replyData;
}

QUrlQuery GDrive::GDriveFileGet::buildUrlArgs(const QString &fields, bool supportsAllDrives)
{
    QUrlQuery query;
    if(!fields.isEmpty()){
        query.addQueryItem("fields",fields);
    }
    if(supportsAllDrives){
        query.addQueryItem("supportsAllDrives",
                           QVariant(supportsAllDrives).toString());
    }
    return query;
}

void GDrive::GDriveFileGet::request_FilesGet(const QUrl &url)
{
    QNetworkRequest request(url);
    request.setRawHeader("Authorization",
                         QByteArray("Bearer " + mp_google->token().toLatin1()));
    auto reply = mp_google->networkAccessManager()->get(request);
    connect(reply,&QNetworkReply::finished,
            this,&GDriveFileGet::on_Request_FilesGet_ReplyFinished);
    connect(reply,QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this,&GDriveFileGet::on_Request_FilesGet_ReplyError);
}

QUrl GDrive::GDriveFileGet::buildUrl(const QString &fileId,
                                     const QString &key,
                                     const QString &fields) const
{
    QUrlQuery query;
    query.addQueryItem("key",key);
    if(!fields.isEmpty()){
        query.addQueryItem("field",fields);
    }
    QUrl url("https://www.googleapis.com/drive/v3/files/" + fileId);
    url.setQuery(query);
    return url;
}

QUrl GDrive::GDriveFileGet::buildUrl(const QString &fileId,
                                     const QString &key,
                                     QUrlQuery args) const
{
    args.addQueryItem("key",key);
    QUrl url("https://www.googleapis.com/drive/v3/files/" + fileId);
    url.setQuery(args);
    return url;
}

void GDrive::GDriveFileGet::on_Request_FilesGet_ReplyFinished()
{
    qInfo() << Q_FUNC_INFO;
    auto reply = qobject_cast<QNetworkReply*>(sender());
    if(reply->error() != QNetworkReply::NoError){
        m_errStr += "Network reply error:" + reply->errorString();
        return;
    }
    m_replyData = reply->readAll();
    taskSucceeded();
    reply->deleteLater();
}

void GDrive::GDriveFileGet::on_Request_FilesGet_ReplyError(QNetworkReply::NetworkError)
{
    qInfo() << Q_FUNC_INFO;
    auto reply = qobject_cast<QNetworkReply*>(sender());
    m_replyData = reply->readAll();
    taskFailed();
    reply->deleteLater();
}
