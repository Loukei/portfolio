#include "gdrivefileget.h"
#include <QOAuth2AuthorizationCodeFlow>
#include <QUrlQuery>

GDrive::GDriveFileGet::GDriveFileGet(QOAuth2AuthorizationCodeFlow *parent,
                                     const QString &fileId,
                                     const QString &fields)
    :GDriveFileTask (parent)
{
    const GDrive::FileGetArgs args = {fileId,false,fields};
    request_FilesGet(args);
}

GDrive::GDriveFileGet::GDriveFileGet(QOAuth2AuthorizationCodeFlow *parent,
                                     const GDrive::FileGetArgs &args)
    :GDriveFileTask (parent)
{
    request_FilesGet(args);
}

GDrive::GDriveFileGet::~GDriveFileGet()
{

}

QByteArray GDrive::GDriveFileGet::getReplyString() const
{
    return m_replyData;
}

void GDrive::GDriveFileGet::request_FilesGet(const FileGetArgs &args)
{
    QUrl url = setupUrl(args);
    QNetworkRequest request(url);
    request.setRawHeader("Authorization",
                         QByteArray("Bearer " + mp_google->token().toLatin1()));
    auto reply = mp_google->networkAccessManager()->get(request);
    connect(reply,&QNetworkReply::finished,
            this,&GDriveFileGet::on_Request_FilesGet_ReplyFinished);
    connect(reply,QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this,&GDriveFileGet::on_Request_FilesGet_ReplyError);
}

QUrl GDrive::GDriveFileGet::setupUrl(const GDrive::FileGetArgs &args)
{
    QUrlQuery query;
    query.addQueryItem("key",mp_google->clientIdentifierSharedKey());
    if(!args.fields().isEmpty()){
        query.addQueryItem("field",args.fields());
    }
//    if(args.acknowledgeAbuse()){
//        query.addQueryItem("acknowledgeAbuse",BooleanToString(args.acknowledgeAbuse()));
//    }
    QUrl url("https://www.googleapis.com/drive/v3/files/" + args.fileId());
    url.setQuery(query);
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
