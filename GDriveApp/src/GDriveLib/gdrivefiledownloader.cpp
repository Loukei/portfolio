#include "gdrivefiledownloader.h"
#include <QOAuth2AuthorizationCodeFlow>
#include <QUrlQuery>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

GDrive::GDriveFileDownloader::GDriveFileDownloader(const QString &fileId,
                                                   const QString &fields,
                                                   QSharedPointer<QFile> file,
                                                   QOAuth2AuthorizationCodeFlow *parent)
    :GDriveFileTask (parent),mp_file(file)
{
    QUrl url = buildUrl(fileId,fields,mp_google->clientIdentifierSharedKey());
    request_Download(url);
}

GDrive::GDriveFileDownloader::GDriveFileDownloader(const QString &fileId,
                                                   const QUrlQuery &args,
                                                   QSharedPointer<QFile> file,
                                                   QOAuth2AuthorizationCodeFlow *parent)
    :GDriveFileTask (parent),mp_file(file)
{
    QUrl url = buildUrl(fileId,"media",mp_google->clientIdentifierSharedKey(),args);
    request_Download(url);
}

GDrive::GDriveFileDownloader::~GDriveFileDownloader()
{
    qInfo() << Q_FUNC_INFO << this;
    mp_file->close();
}

QByteArray GDrive::GDriveFileDownloader::getReplyString() const
{
    return m_replyData;
}

QUrlQuery GDrive::GDriveFileDownloader::buildUrlArgs(bool acknowledgeAbuse, const QString &fields, bool supportsAllDrives)
{
    QUrlQuery args;
    if(acknowledgeAbuse){
        args.addQueryItem("acknowledgeAbuse",QVariant(acknowledgeAbuse).toString());
    }
    if(!fields.isEmpty()){
        args.addQueryItem("field",fields);
    }
    if(supportsAllDrives){
        args.addQueryItem("supportsAllDrives",QVariant(supportsAllDrives).toString());
    }
    return args;
}

void GDrive::GDriveFileDownloader::request_Download(const QUrl &url)
{
    QNetworkRequest request(url);
    request.setRawHeader("Authorization",
                         QByteArray("Bearer " + mp_google->token().toLatin1()));
    auto reply = mp_google->networkAccessManager()->get(request);
    connect(reply,&QNetworkReply::finished,
            this,&GDriveFileDownloader::on_Download_ReplyFinished);
    connect(reply,QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this,&GDriveFileDownloader::on_Download_ReplyError);
}

QUrl GDrive::GDriveFileDownloader::buildUrl(const QString &fileId,
                                            const QString &fields,
                                            const QString &key) const
{
    QUrlQuery query;
    query.addQueryItem("alt","media");
    if(!fields.isEmpty()){
        query.addQueryItem("fields",fields);
    }
    query.addQueryItem("key",key);
    QUrl url("https://www.googleapis.com/drive/v3/files/" + fileId);
    url.setQuery(query);
    return url;
}

QUrl GDrive::GDriveFileDownloader::buildUrl(const QString &fileId,
                                            const QString &alt,
                                            const QString &key,
                                            QUrlQuery args) const
{
    args.addQueryItem("alt",alt);
    args.addQueryItem("key",key);
    QUrl url("https://www.googleapis.com/drive/v3/files/" + fileId);
    url.setQuery(args);
    return url;
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
    qInfo() << Q_FUNC_INFO;
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
    qInfo() << Q_FUNC_INFO;
    auto reply = qobject_cast<QNetworkReply*>(sender());
    m_errStr += getErrorMessage(reply);
    taskFailed();
    reply->deleteLater();
}
