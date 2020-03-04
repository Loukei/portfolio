#include "gdrivedownloader.h"
#include <QOAuth2AuthorizationCodeFlow>
#include <QUrlQuery>

GDrive::GDriveDownloader::GDriveDownloader(QOAuth2AuthorizationCodeFlow *parent,
        const QString &fileId,
        QFile *file,
        const QString fields):QObject (parent),mp_google(parent),mp_file(file)
{
    request_Download(fileId,fields);
}

GDrive::GDriveDownloader::~GDriveDownloader()
{

}

QString GDrive::GDriveDownloader::errorString()
{
    return m_errStr;
}

int GDrive::GDriveDownloader::getSleepTime(int collisions)
{
    //! todo
    return collisions;
}

void GDrive::GDriveDownloader::request_Download(const QString &fileId, const QString fields)
{
    QUrlQuery query;
    query.addQueryItem("alt","media");
    query.addQueryItem("field",fields);//! field could be empty
    query.addQueryItem("key",mp_google->clientIdentifierSharedKey());
    QUrl url("https://www.googleapis.com/drive/v3/files/" + fileId);
    url.setQuery(query);
    QNetworkRequest request(url);
    request.setRawHeader("Authorization",
                         QByteArray("Bearer " + mp_google->token().toLatin1()));
    auto reply = mp_google->networkAccessManager()->get(request);
    connect(reply,&QNetworkReply::finished,
            this,&GDriveDownloader::on_Download_ReplyFinished);
    connect(reply,QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this,&GDriveDownloader::on_Download_ReplyError);
}

void GDrive::GDriveDownloader::on_Download_ReplyFinished()
{
    auto reply = qobject_cast<QNetworkReply*>(sender());
    if(reply->error() == QNetworkReply::NoError){
        qWarning() << "[Error]GDriveDownloader error:" << reply->errorString();
        return;
    }
    qInfo() << "Info: " << reply->readAll();
    reply->deleteLater();
}

void GDrive::GDriveDownloader::on_Download_ReplyError(QNetworkReply::NetworkError)
{
    auto reply = qobject_cast<QNetworkReply*>(sender());
    auto httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    qWarning() << "[Error]Http status:" << httpStatus
               << " errorString:" << reply->errorString()
               << " data:" << reply->readAll();
    reply->deleteLater();
}
