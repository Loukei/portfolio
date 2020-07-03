#include "gdrivefiledownloader.h"
#include <QOAuth2AuthorizationCodeFlow>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>

GDrive::GDriveFileDownloader::GDriveFileDownloader(const QString &fileId,
                                                   const QString &fields,
                                                   const QString &filepath,
                                                   QOAuth2AuthorizationCodeFlow *parent)
    :GDriveFileTask (parent),m_file(filepath,this)
{
    m_requestUrl = buildUrl(fileId,fields,mp_google->clientIdentifierSharedKey());
}

GDrive::GDriveFileDownloader::GDriveFileDownloader(const QString &fileId,
                                                   const QUrlQuery &args,
                                                   const QString &filepath,
                                                   QOAuth2AuthorizationCodeFlow *parent)
    :GDriveFileTask (parent),m_file(filepath,this)
{
    m_requestUrl = buildUrl(fileId,"media",mp_google->clientIdentifierSharedKey(),args);
}

GDrive::GDriveFileDownloader::~GDriveFileDownloader()
{
    m_file.commit();
}

bool GDrive::GDriveFileDownloader::start()
{
    if(!m_file.open(QIODevice::WriteOnly)){
        m_errStr += QString("%1 open fail: %2").arg(m_file.fileName(),m_file.errorString());
        taskFailed();
        return false;
    }
    m_currentReply = requestDownload(buildRequest(m_requestUrl));
    return true;
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

void GDrive::GDriveFileDownloader::abort()
{
    qInfo() << Q_FUNC_INFO;
    if(m_currentReply){
        m_currentReply->disconnect();
        m_currentReply->abort();
        m_currentReply->deleteLater();
    }else {
        qInfo() << "No network request is running";
    }
    m_file.cancelWriting();
    m_file.commit();
    m_errStr += "User download cancled.";
    taskFailed();
}

bool GDrive::GDriveFileDownloader::checkAndOpenFile()
{
    if(!m_file.open(QIODevice::WriteOnly)){
        m_errStr += QString("%1 open fail: %2").arg(m_file.fileName(),m_file.errorString());
        return false;
    }
    return true;
}

QNetworkReply *GDrive::GDriveFileDownloader::requestDownload(const QNetworkRequest &request)
{
    auto reply = mp_google->networkAccessManager()->get(request);
    connect(reply,&QNetworkReply::finished,
            this,&GDriveFileDownloader::onDownloadReplyFinished);
    connect(reply,QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this,&GDriveFileDownloader::onDownloadReplyError);
    connect(reply,&QNetworkReply::readyRead,
            this,&GDriveFileDownloader::onDownloadReadyread);
    connect(reply,&QNetworkReply::downloadProgress,
            this,&GDriveFileDownloader::downloadProgress);
    return reply;
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

QNetworkRequest GDrive::GDriveFileDownloader::buildRequest(const QUrl &url) const
{
    QNetworkRequest request(url);
    request.setRawHeader("Authorization",
                         QByteArray("Bearer " + mp_google->token().toLatin1()));
    return request;
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

void GDrive::GDriveFileDownloader::onDownloadReplyFinished()
{
    qInfo() << Q_FUNC_INFO;
    auto reply = qobject_cast<QNetworkReply*>(sender());
    if(reply->error() != QNetworkReply::NoError){
        m_errStr += "Network reply error:" + reply->errorString();
        return;
    }
    //! write reply content to mp_file, return false if write failed
    if(m_file.commit()){
        reply->deleteLater();
        taskSucceeded();
    }else {
        reply->deleteLater();
        m_errStr += QString("File commit %1").arg(m_file.errorString());
        taskFailed();
    }
}

void GDrive::GDriveFileDownloader::onDownloadReplyError(QNetworkReply::NetworkError)
{
    auto reply = qobject_cast<QNetworkReply*>(sender());
    m_errStr += getErrorMessage(reply);
    m_file.cancelWriting();
    m_file.commit();
    reply->deleteLater();
    taskFailed();
}

void GDrive::GDriveFileDownloader::onDownloadReadyread()
{
    auto reply = qobject_cast<QNetworkReply*>(sender());
    m_file.write(reply->readAll());
}
