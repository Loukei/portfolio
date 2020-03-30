#include "gdrivefileget.h"
#include <QOAuth2AuthorizationCodeFlow>
#include <QUrlQuery>

GDrive::GDriveFileGet::GDriveFileGet(QOAuth2AuthorizationCodeFlow *parent,
                                     const QString &fileId,
                                     const QString &fields):
    GDriveFileTask (parent)
{
    request_FilesGet(fileId,fields);
}

GDrive::GDriveFileGet::~GDriveFileGet()
{

}

QByteArray GDrive::GDriveFileGet::getReplyString() const
{
    return m_replyData;
}

void GDrive::GDriveFileGet::request_FilesGet(const QString &fileId, const QString &fields)
{
    QUrlQuery query;
    query.addQueryItem("key",mp_google->clientIdentifierSharedKey());
    if(!fields.isEmpty()){
        query.addQueryItem("field",fields);
    }
    QUrl url("https://www.googleapis.com/drive/v3/files/" + fileId);
    url.setQuery(query);
    QNetworkRequest request(url);
    request.setRawHeader("Authorization",
                         QByteArray("Bearer " + mp_google->token().toLatin1()));
    auto reply = mp_google->networkAccessManager()->get(request);
    connect(reply,&QNetworkReply::finished,
            this,&GDriveFileGet::on_Request_FilesGet_ReplyFinished);
    connect(reply,QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this,&GDriveFileGet::on_Request_FilesGet_ReplyFinished);
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
    m_isFailed = false;
    m_isComplete = true;
    emit finished();
    reply->deleteLater();
}

void GDrive::GDriveFileGet::on_Request_FilesGet_ReplyError(QNetworkReply::NetworkError)
{
    qInfo() << Q_FUNC_INFO;
    auto reply = qobject_cast<QNetworkReply*>(sender());
    m_replyData = reply->readAll();
    m_isFailed = true;
    m_isComplete = true;
    emit finished();
    reply->deleteLater();
}
