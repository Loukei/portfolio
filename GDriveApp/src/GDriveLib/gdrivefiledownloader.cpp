#include "gdrivefiledownloader.h"
#include <QOAuth2AuthorizationCodeFlow>
#include <QUrlQuery>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

GDrive::GDriveFileDownloader::GDriveFileDownloader(QOAuth2AuthorizationCodeFlow *parent,
        const QString &fileId,
        const QString &fields,
        QSharedPointer<QFile> file)
    :GDriveFileTask (parent),mp_file(file)
{
    request_Download(fileId,fields);
}

GDrive::GDriveFileDownloader::~GDriveFileDownloader()
{
    mp_file->close();
    qInfo() << "GDriveFileDownloader::~GDriveFileDownloader " << this;
}

void GDrive::GDriveFileDownloader::request_Download(const QString &fileId, const QString &fields)
{
    QUrlQuery query;
    query.addQueryItem("key",mp_google->clientIdentifierSharedKey());
    query.addQueryItem("alt","media");
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
            this,&GDriveFileDownloader::on_Download_ReplyFinished);
    connect(reply,QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this,&GDriveFileDownloader::on_Download_ReplyError);
}

bool GDrive::GDriveFileDownloader::writeFile(QNetworkReply *reply)
{
    //! open file
    if(!mp_file->open(QIODevice::WriteOnly)){
        m_errStr += mp_file->errorString();
        return false;
    }
    //! write reply content to file
    qint64 writebyte = mp_file->write(reply->readAll());
    if(writebyte == -1){ //! write error
        m_errStr += mp_file->errorString();
        return false;
    }
    return true;
}

QString GDrive::GDriveFileDownloader::getErrorMessage(QNetworkReply *reply)
{
    auto jsonDoc = QJsonDocument::fromJson(reply->readAll());
    if(jsonDoc.isNull()){
        return "Network reply message parse error";
    }
    auto errorObj = jsonDoc["error"].toObject();
    QString errMsg("Network reply\n");
    errMsg += QString("code: %1\n").arg(errorObj.value("code").toInt());
    errMsg += QString("message: %2\n").arg(errorObj.value("message").toString());
    return errMsg;
}

void GDrive::GDriveFileDownloader::on_Download_ReplyFinished()
{
    qInfo() << "GDrive::GDriveFileDownloader::on_Download_ReplyFinished()";
    auto reply = qobject_cast<QNetworkReply*>(sender());
    if(reply->error() != QNetworkReply::NoError){
        m_errStr += "Network reply error:" + reply->errorString();
        return;
    }
    //! write reply content to mp_file, return false if write failed
    m_isFailed = !writeFile(reply);
    m_isComplete = true;
    emit finished();
    reply->deleteLater();
}

void GDrive::GDriveFileDownloader::on_Download_ReplyError(QNetworkReply::NetworkError)
{
    qInfo() << "GDrive::GDriveFileDownloader::on_Download_ReplyError()";
    auto reply = qobject_cast<QNetworkReply*>(sender());
    m_errStr += getErrorMessage(reply);
    m_isFailed = true;
    m_isComplete = true;
    emit finished();
    reply->deleteLater();
}
